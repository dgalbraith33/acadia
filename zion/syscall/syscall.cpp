#include "syscall/syscall.h"

#include <stdint.h>

#include "debug/debug.h"
#include "include/zcall.h"
#include "scheduler/scheduler.h"

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
  return sched::CurrentThread().Rsp0Start();
}

void InitSyscall() {
  uint64_t efer_val = GetMSR(EFER);
  efer_val |= 1;
  SetMSR(EFER, efer_val);
  if (GetMSR(EFER) != efer_val) {
    panic("Failed to set EFER MSR");
  }

  uint64_t star_val = GetMSR(STAR);
  // FIXME: Fix GDT such that we can properly set the user CS.
  // Due to the ability to jump from a 64 bit kernel into compatibility mode,
  // we set the user_cs to the kernel_cs because it adds 16 to jump to 64-bit
  // mode. See AMD Manual 3.4 instruction SYSRET for more info.
  uint64_t kernel_cs = 0x8;
  uint64_t user_cs = kernel_cs;
  star_val |= (kernel_cs << 32) | (user_cs << 48);
  SetMSR(STAR, star_val);
  SetMSR(LSTAR, reinterpret_cast<uint64_t>(syscall_enter));
}

extern "C" void SyscallHandler(uint64_t call_id, char* message) {
  Thread& thread = sched::CurrentThread();
  switch (call_id) {
    case Z_THREAD_EXIT:
      thread.Exit();
      panic("Returned from thread exit");
      break;
    case Z_DEBUG_PRINT:
      dbgln("[%u.%u] [Debug] %s", thread.pid(), thread.tid(), message);
      break;
    default:
      panic("Unhandled syscall number: %u", call_id);
  }
}
