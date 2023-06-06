#include "include/zcall.h"

#include <stdint.h>

uint64_t SysCall0(uint64_t number) {
  uint64_t return_code;
  asm("syscall" : "=a"(return_code) : "D"(number));
  return return_code;
}

uint64_t SysCall1(uint64_t number, const void* first) {
  uint64_t return_code;
  asm("syscall" : "=a"(return_code) : "D"(number), "S"(first) : "rcx", "r11");
  return return_code;
}

uint64_t SysCall2(uint64_t number, const void* first, const void* second) {
  uint64_t return_code;
  asm("syscall"
      : "=a"(return_code)
      : "D"(number), "S"(first), "d"(second)
      : "rcx", "r11");
  return return_code;
}

uint64_t ZDebug(const char* message) {
  return SysCall1(Z_DEBUG_PRINT, message);
}

uint64_t ZProcessSpawnElf(uint64_t cap_id, uint64_t elf_base,
                          uint64_t elf_size) {
  ZProcessSpawnElfReq req{
      .cap_id = cap_id,
      .elf_base = elf_base,
      .elf_size = elf_size,
  };
  return SysCall1(Z_PROCESS_SPAWN, &req);
}

uint64_t ZThreadCreate(uint64_t proc_cap, uint64_t* thread_cap) {
  ZThreadCreateReq req{
      .proc_cap = proc_cap,
  };
  ZThreadCreateResp resp;
  uint64_t ret = SysCall2(Z_THREAD_CREATE, &req, &resp);
  *thread_cap = resp.thread_cap;
  return ret;
}

uint64_t ZThreadStart(uint64_t thread_cap, uint64_t entry, uint64_t arg1,
                      uint64_t arg2) {
  ZThreadStartReq req{
      .thread_cap = thread_cap,
      .entry = entry,
      .arg1 = arg1,
      .arg2 = arg2,
  };
  return SysCall1(Z_THREAD_START, &req);
}

void ZThreadExit() { SysCall0(Z_THREAD_EXIT); }
