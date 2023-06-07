#include "syscall/syscall.h"

#include <stdint.h>

#include "debug/debug.h"
#include "include/zcall.h"
#include "include/zerrors.h"
#include "object/channel.h"
#include "object/process.h"
#include "scheduler/process_manager.h"
#include "scheduler/scheduler.h"
#include "usr/zcall_internal.h"

#define EFER 0xC0000080
#define STAR 0xC0000081
#define LSTAR 0xC0000082

namespace {

uint64_t GetMSR(uint32_t msr) {
  uint32_t lo, hi;
  asm("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
  return (static_cast<uint64_t>(hi) << 32) | lo;
}

void SetMSR(uint32_t msr, uint64_t val) {
  uint32_t lo = static_cast<uint32_t>(val);
  uint32_t hi = val >> 32;
  asm("wrmsr" ::"a"(lo), "d"(hi), "c"(msr));
}

extern "C" void syscall_enter();

}  // namespace

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
  chan->Read(req->message);
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
    case Z_CHANNEL_CREATE:
      return ChannelCreate(reinterpret_cast<ZChannelCreateResp*>(resp));
    case Z_CHANNEL_SEND:
      return ChannelSend(reinterpret_cast<ZChannelSendReq*>(req));
    case Z_CHANNEL_RECV:
      return ChannelRecv(reinterpret_cast<ZChannelRecvReq*>(req));
    case Z_DEBUG_PRINT:
      dbgln("[Debug] %s", req);
      return Z_OK;
      break;
    default:
      panic("Unhandled syscall number: %x", call_id);
  }
  UNREACHABLE
}
