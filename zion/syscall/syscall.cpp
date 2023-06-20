#include "syscall/syscall.h"

#include <stdint.h>

#include "common/msr.h"
#include "debug/debug.h"
#include "include/zcall.h"
#include "interrupt/interrupt.h"
#include "memory/physical_memory.h"
#include "object/channel.h"
#include "object/port.h"
#include "object/process.h"
#include "scheduler/process_manager.h"
#include "scheduler/scheduler.h"
#include "syscall/address_space.h"
#include "syscall/memory_object.h"
#include "syscall/process.h"
#include "syscall/thread.h"
#include "usr/zcall_internal.h"

#define EFER 0xC0000080
#define STAR 0xC0000081
#define LSTAR 0xC0000082

extern "C" void syscall_enter();

// Used by syscall_enter.s
extern "C" uint64_t GetKernelRsp() {
  return gScheduler->CurrentThread()->Rsp0Start();
}

void InitSyscall() {
  uint64_t efer_val = GetMSR(EFER);
  efer_val |= 1;
  SetMSR(EFER, efer_val);
  if (GetMSR(EFER) != efer_val) {
    panic("Failed to set EFER MSR");
  }

  uint64_t star_val = GetMSR(STAR);
  uint64_t kernel_cs = 0x8;
  // Due to the ability to jump from a 64 bit kernel into compatibility mode,
  // this will actually use CS 0x20 (and SS 0x18).
  // See AMD Manual 3.4 instruction SYSRET for more info.
  uint64_t user_cs = 0x18;
  star_val |= (kernel_cs << 32) | (user_cs << 48);
  SetMSR(STAR, star_val);
  SetMSR(LSTAR, reinterpret_cast<uint64_t>(syscall_enter));
}

z_err_t ValidateCap(const RefPtr<Capability>& cap, uint64_t permissions) {
  if (!cap) {
    return Z_ERR_CAP_NOT_FOUND;
  }
  // FIXME: Check capability type before permissions, otherwise you can
  // get a confusing error.
  if (!cap->HasPermissions(permissions)) {
    dbgln("PERM, has %x needs %x", cap->permissions(), permissions);
    return Z_ERR_CAP_DENIED;
  }
  return Z_OK;
}

z_err_t ChannelCreate(ZChannelCreateResp* resp) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_pair = Channel::CreateChannelPair();
  resp->chan_cap1 =
      proc.AddNewCapability(chan_pair.first(), ZC_WRITE | ZC_READ);
  resp->chan_cap2 =
      proc.AddNewCapability(chan_pair.second(), ZC_WRITE | ZC_READ);
  return Z_OK;
}

z_err_t ChannelSend(ZChannelSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCap(chan_cap, ZC_WRITE));

  auto chan = chan_cap->obj<Channel>();
  RET_IF_NULL(chan);
  chan->Write(req->message);
  return Z_OK;
}

z_err_t ChannelRecv(ZChannelRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCap(chan_cap, ZC_READ));

  auto chan = chan_cap->obj<Channel>();
  RET_IF_NULL(chan);
  return chan->Read(req->message);
}

z_err_t PortCreate(ZPortCreateResp* resp) {
  auto& proc = gScheduler->CurrentProcess();
  auto port = MakeRefCounted<Port>();
  resp->port_cap = proc.AddNewCapability(port, ZC_WRITE | ZC_READ);
  return Z_OK;
}

z_err_t PortSend(ZPortSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCap(port_cap, ZC_WRITE));

  auto port = port_cap->obj<Port>();
  RET_IF_NULL(port);
  return port->Write(req->message);
}

z_err_t PortRecv(ZPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCap(port_cap, ZC_READ));

  auto port = port_cap->obj<Port>();
  RET_IF_NULL(port);
  return port->Read(req->message);
}

z_err_t PortPoll(ZPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCap(port_cap, ZC_READ));

  auto port = port_cap->obj<Port>();
  RET_IF_NULL(port);
  if (!port->HasMessages()) {
    return Z_ERR_EMPTY;
  }
  return port->Read(req->message);
}

z_err_t IrqRegister(ZIrqRegisterReq* req, ZIrqRegisterResp* resp) {
  auto& proc = gScheduler->CurrentProcess();
  if (req->irq_num != Z_IRQ_PCI_BASE) {
    // FIXME: Don't hardcode this nonsense.
    return Z_ERR_UNIMPLEMENTED;
  }
  RefPtr<Port> port = MakeRefCounted<Port>();
  resp->port_cap = proc.AddNewCapability(port, ZC_READ | ZC_WRITE);
  RegisterPciPort(port);
  return Z_OK;
}

z_err_t CapDuplicate(ZCapDuplicateReq* req, ZCapDuplicateResp* resp) {
  auto& proc = gScheduler->CurrentProcess();
  auto cap = proc.GetCapability(req->cap);
  if (!cap) {
    return Z_ERR_CAP_NOT_FOUND;
  }
  resp->cap = proc.AddExistingCapability(cap);
  return Z_OK;
}

#define CASE(name)  \
  case kZion##name: \
    return name(reinterpret_cast<Z##name##Req*>(req));

extern "C" z_err_t SyscallHandler(uint64_t call_id, void* req, void* resp) {
  RefPtr<Thread> thread = gScheduler->CurrentThread();
  switch (call_id) {
    // syscall/process.h
    CASE(ProcessExit);
    CASE(ProcessSpawn);
    // syscall/thread.h
    CASE(ThreadCreate);
    CASE(ThreadStart);
    CASE(ThreadExit);
    // syscall/address_space.h
    CASE(AddressSpaceMap);
    // syscall/memory_object.h
    CASE(MemoryObjectCreate);
    CASE(MemoryObjectCreatePhysical);
    CASE(MemoryObjectCreateContiguous);
    CASE(TempPcieConfigObjectCreate);
    case Z_CHANNEL_CREATE:
      return ChannelCreate(reinterpret_cast<ZChannelCreateResp*>(resp));
    case Z_CHANNEL_SEND:
      return ChannelSend(reinterpret_cast<ZChannelSendReq*>(req));
    case Z_CHANNEL_RECV:
      return ChannelRecv(reinterpret_cast<ZChannelRecvReq*>(req));
    case Z_PORT_CREATE:
      return PortCreate(reinterpret_cast<ZPortCreateResp*>(resp));
    case Z_PORT_SEND:
      return PortSend(reinterpret_cast<ZPortSendReq*>(req));
    case Z_PORT_RECV:
      return PortRecv(reinterpret_cast<ZPortRecvReq*>(req));
    case Z_PORT_POLL:
      return PortPoll(reinterpret_cast<ZPortRecvReq*>(req));
    case Z_IRQ_REGISTER:
      return IrqRegister(reinterpret_cast<ZIrqRegisterReq*>(req),
                         reinterpret_cast<ZIrqRegisterResp*>(resp));
    case Z_CAP_DUPLICATE:
      return CapDuplicate(reinterpret_cast<ZCapDuplicateReq*>(req),
                          reinterpret_cast<ZCapDuplicateResp*>(resp));
    case Z_DEBUG_PRINT:
      dbgln("[Debug] %s", req);
      return Z_OK;
      break;
    default:
      panic("Unhandled syscall number: %x", call_id);
  }
  UNREACHABLE
}
