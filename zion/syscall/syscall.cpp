#include "syscall/syscall.h"

#include <stdint.h>

#include "boot/acpi.h"
#include "common/msr.h"
#include "debug/debug.h"
#include "include/zcall.h"
#include "include/zerrors.h"
#include "interrupt/interrupt.h"
#include "memory/physical_memory.h"
#include "object/channel.h"
#include "object/port.h"
#include "object/process.h"
#include "scheduler/process_manager.h"
#include "scheduler/scheduler.h"
#include "usr/zcall_internal.h"

#define EFER 0xC0000080
#define STAR 0xC0000081
#define LSTAR 0xC0000082

extern "C" void syscall_enter();

// Used by syscall_enter.s
extern "C" uint64_t GetKernelRsp() {
  return gScheduler->CurrentThread().Rsp0Start();
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

z_err_t ValidateCap(const RefPtr<Capability>& cap, Capability::Type type,
                    uint64_t permissions) {
  if (!cap) {
    return Z_ERR_CAP_NOT_FOUND;
  }
  if (!cap->CheckType(type)) {
    return Z_ERR_CAP_TYPE;
  }
  if (!cap->HasPermissions(permissions)) {
    return Z_ERR_CAP_DENIED;
  }
  return Z_OK;
}

z_err_t ProcessSpawn(ZProcessSpawnReq* req, ZProcessSpawnResp* resp) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->proc_cap);
  RET_ERR(ValidateCap(cap, Capability::PROCESS, ZC_PROC_SPAWN_PROC));

  RefPtr<Process> proc = Process::Create();
  gProcMan->InsertProcess(proc);

  resp->proc_cap = curr_proc.AddCapability(proc);
  resp->vmas_cap = curr_proc.AddCapability(proc->vmas());

  if (req->bootstrap_cap != 0) {
    auto cap = curr_proc.ReleaseCapability(req->bootstrap_cap);
    if (!cap) {
      return Z_ERR_CAP_NOT_FOUND;
    }
    // FIXME: Check permissions.
    resp->bootstrap_cap = proc->AddCapability(cap);
  }

  return Z_OK;
}

z_err_t ThreadCreate(ZThreadCreateReq* req, ZThreadCreateResp* resp) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->proc_cap);
  RET_ERR(ValidateCap(cap, Capability::PROCESS, ZC_PROC_SPAWN_THREAD));

  auto parent_proc = cap->obj<Process>();
  auto thread = parent_proc->CreateThread();
  resp->thread_cap = curr_proc.AddCapability(thread);

  return Z_OK;
}

z_err_t ThreadStart(ZThreadStartReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->thread_cap);
  RET_ERR(ValidateCap(cap, Capability::THREAD, ZC_WRITE));

  auto thread = cap->obj<Thread>();
  // FIXME: validate entry point is in user space.
  thread->Start(req->entry, req->arg1, req->arg2);
  return Z_OK;
}

z_err_t AddressSpaceMap(ZAddressSpaceMapReq* req, ZAddressSpaceMapResp* resp) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto vmas_cap = curr_proc.GetCapability(req->vmas_cap);
  auto vmmo_cap = curr_proc.GetCapability(req->vmmo_cap);
  RET_ERR(ValidateCap(vmas_cap, Capability::ADDRESS_SPACE, ZC_WRITE));
  RET_ERR(ValidateCap(vmmo_cap, Capability::MEMORY_OBJECT, ZC_WRITE));

  auto vmas = vmas_cap->obj<AddressSpace>();
  auto vmmo = vmmo_cap->obj<MemoryObject>();
  // FIXME: Validation necessary.
  if (req->vmas_offset != 0) {
    vmas->MapInMemoryObject(req->vmas_offset, vmmo);
    resp->vaddr = req->vmas_offset;
  } else {
    resp->vaddr = vmas->MapInMemoryObject(vmmo);
  }
  return Z_OK;
}

z_err_t MemoryObjectCreate(ZMemoryObjectCreateReq* req,
                           ZMemoryObjectCreateResp* resp) {
  auto& curr_proc = gScheduler->CurrentProcess();
  resp->vmmo_cap =
      curr_proc.AddCapability(MakeRefCounted<MemoryObject>(req->size));
  return Z_OK;
}

z_err_t MemoryObjectCreatePhysical(ZMemoryObjectCreatePhysicalReq* req,
                                   ZMemoryObjectCreatePhysicalResp* resp) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t paddr = req->paddr;
  if (paddr == 0) {
    paddr = phys_mem::AllocateContinuous(((req->size - 1) / 0x1000) + 1);
  }
  auto vmmo_ref = MakeRefCounted<FixedMemoryObject>(paddr, req->size);
  resp->vmmo_cap =
      curr_proc.AddCapability(StaticCastRefPtr<MemoryObject>(vmmo_ref));
  resp->paddr = paddr;
  return Z_OK;
}

z_err_t TempPcieConfigObjectCreate(ZTempPcieConfigObjectCreateResp* resp) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t pci_base, pci_size;
  RET_ERR(GetPciExtendedConfiguration(&pci_base, &pci_size));
  auto vmmo_ref = MakeRefCounted<FixedMemoryObject>(pci_base, pci_size);
  resp->vmmo_cap =
      curr_proc.AddCapability(StaticCastRefPtr<MemoryObject>(vmmo_ref));
  resp->vmmo_size = pci_size;
  return Z_OK;
}

z_err_t ChannelCreate(ZChannelCreateResp* resp) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_pair = Channel::CreateChannelPair();
  resp->chan_cap1 = proc.AddCapability(chan_pair.first());
  resp->chan_cap2 = proc.AddCapability(chan_pair.second());
  return Z_OK;
}

z_err_t ChannelSend(ZChannelSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCap(chan_cap, Capability::CHANNEL, ZC_WRITE));

  auto chan = chan_cap->obj<Channel>();
  chan->Write(req->message);
  return Z_OK;
}

z_err_t ChannelRecv(ZChannelRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCap(chan_cap, Capability::CHANNEL, ZC_READ));

  auto chan = chan_cap->obj<Channel>();
  return chan->Read(req->message);
}

z_err_t PortRecv(ZPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  dbgln("Port cap %u", req->port_cap);
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCap(port_cap, Capability::PORT, ZC_READ));

  auto port = port_cap->obj<Port>();
  return port->Read(req->message);
}

z_err_t IrqRegister(ZIrqRegisterReq* req, ZIrqRegisterResp* resp) {
  auto& proc = gScheduler->CurrentProcess();
  if (req->irq_num != Z_IRQ_PCI_BASE) {
    // FIXME: Don't hardcode this nonsense.
    dbgln("Irq %x", req->irq_num);
    return Z_ERR_UNIMPLEMENTED;
  }
  RefPtr<Port> port = MakeRefCounted<Port>();
  resp->port_cap = proc.AddCapability(port);
  dbgln("Port cap %u", resp->port_cap);
  RegisterPciPort(port);
  return Z_OK;
}

extern "C" z_err_t SyscallHandler(uint64_t call_id, void* req, void* resp) {
  Thread& thread = gScheduler->CurrentThread();
  switch (call_id) {
    case Z_PROCESS_EXIT:
      // FIXME: kill process here.
      dbgln("Exit code: %u", req);
      thread.Exit();
      panic("Returned from thread exit");
      break;
    case Z_PROCESS_SPAWN:
      return ProcessSpawn(reinterpret_cast<ZProcessSpawnReq*>(req),
                          reinterpret_cast<ZProcessSpawnResp*>(resp));
    case Z_THREAD_CREATE:
      return ThreadCreate(reinterpret_cast<ZThreadCreateReq*>(req),
                          reinterpret_cast<ZThreadCreateResp*>(resp));
    case Z_THREAD_START:
      return ThreadStart(reinterpret_cast<ZThreadStartReq*>(req));
    case Z_THREAD_EXIT:
      thread.Exit();
      panic("Returned from thread exit");
      break;

    case Z_ADDRESS_SPACE_MAP:
      return AddressSpaceMap(reinterpret_cast<ZAddressSpaceMapReq*>(req),
                             reinterpret_cast<ZAddressSpaceMapResp*>(resp));
    case Z_MEMORY_OBJECT_CREATE:
      return MemoryObjectCreate(
          reinterpret_cast<ZMemoryObjectCreateReq*>(req),
          reinterpret_cast<ZMemoryObjectCreateResp*>(resp));
    case Z_MEMORY_OBJECT_CREATE_PHYSICAL:
      return MemoryObjectCreatePhysical(
          reinterpret_cast<ZMemoryObjectCreatePhysicalReq*>(req),
          reinterpret_cast<ZMemoryObjectCreatePhysicalResp*>(resp));
    case Z_TEMP_PCIE_CONFIG_OBJECT_CREATE:
      return TempPcieConfigObjectCreate(
          reinterpret_cast<ZTempPcieConfigObjectCreateResp*>(resp));
    case Z_CHANNEL_CREATE:
      return ChannelCreate(reinterpret_cast<ZChannelCreateResp*>(resp));
    case Z_CHANNEL_SEND:
      return ChannelSend(reinterpret_cast<ZChannelSendReq*>(req));
    case Z_CHANNEL_RECV:
      return ChannelRecv(reinterpret_cast<ZChannelRecvReq*>(req));
    case Z_PORT_RECV:
      return PortRecv(reinterpret_cast<ZPortRecvReq*>(req));
    case Z_IRQ_REGISTER:
      return IrqRegister(reinterpret_cast<ZIrqRegisterReq*>(req),
                         reinterpret_cast<ZIrqRegisterResp*>(resp));
    case Z_DEBUG_PRINT:
      dbgln("[Debug] %s", req);
      return Z_OK;
      break;
    default:
      panic("Unhandled syscall number: %x", call_id);
  }
  UNREACHABLE
}
