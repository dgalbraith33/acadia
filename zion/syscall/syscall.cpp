#include "syscall/syscall.h"

#include <stdint.h>

#include "common/msr.h"
#include "debug/debug.h"
#include "scheduler/scheduler.h"
#include "syscall/address_space.h"
#include "syscall/capability.h"
#include "syscall/debug.h"
#include "syscall/ipc.h"
#include "syscall/memory_object.h"
#include "syscall/process.h"
#include "syscall/synchronization.h"
#include "syscall/thread.h"

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

#define CASE(name)  \
  case kZion##name: \
    return name(reinterpret_cast<Z##name##Req*>(req));

extern "C" z_err_t SyscallHandler(uint64_t call_id, void* req) {
  switch (call_id) {
    // syscall/process.h
    CASE(ProcessExit);
    CASE(ProcessSpawn);
    CASE(ProcessWait);
    // syscall/thread.h
    CASE(ThreadCreate);
    CASE(ThreadStart);
    CASE(ThreadExit);
    CASE(ThreadWait);
    CASE(ThreadSleep);
    // syscall/address_space.h
    CASE(AddressSpaceMap);
    CASE(AddressSpaceUnmap);
    // syscall/memory_object.h
    CASE(MemoryObjectCreate);
    CASE(MemoryObjectCreatePhysical);
    CASE(MemoryObjectCreateContiguous);
    CASE(MemoryObjectDuplicate);
    CASE(MemoryObjectInspect);
    // syscall/ipc.h
    CASE(ChannelCreate);
    CASE(ChannelSend);
    CASE(ChannelRecv);
    CASE(PortCreate);
    CASE(PortSend);
    CASE(PortRecv);
    CASE(PortPoll);
    CASE(IrqRegister);
    CASE(EndpointCreate);
    CASE(EndpointSend);
    CASE(EndpointRecv);
    CASE(ReplyPortSend);
    CASE(ReplyPortRecv);
    // syscall/capability.h
    CASE(CapDuplicate);
    CASE(CapRelease);
    // syscall/syncronization.h
    CASE(MutexCreate);
    CASE(MutexLock);
    CASE(MutexRelease);
    CASE(SemaphoreCreate);
    CASE(SemaphoreWait);
    CASE(SemaphoreSignal);
    // syscall/debug.h
    CASE(Debug);
    default:
      dbgln("Unhandled syscall number: {x}", call_id);
      return glcr::UNIMPLEMENTED;
  }
  UNREACHABLE
}
