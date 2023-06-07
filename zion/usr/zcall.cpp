#include "include/zcall.h"

#include <stdint.h>

#include "usr/zcall_internal.h"

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

void ZProcessExit(uint64_t code) {
  SysCall1(Z_PROCESS_EXIT, reinterpret_cast<void*>(code));
}

uint64_t ZProcessSpawn(uint64_t proc_cap, uint64_t* new_proc_cap,
                       uint64_t* new_vmas_cap) {
  ZProcessSpawnReq req{
      .proc_cap = proc_cap,
  };
  ZProcessSpawnResp resp;
  uint64_t ret = SysCall2(Z_PROCESS_SPAWN, &req, &resp);
  *new_proc_cap = resp.proc_cap;
  *new_vmas_cap = resp.vmas_cap;
  return ret;
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

uint64_t ZAddressSpaceMap(uint64_t vmas_cap, uint64_t vmas_offset,
                          uint64_t vmmo_cap, uint64_t* vaddr) {
  ZAddressSpaceMapReq req{
      .vmas_cap = vmas_cap,
      .vmas_offset = vmas_offset,
      .vmmo_cap = vmmo_cap,
  };
  ZAddressSpaceMapResp resp;
  uint64_t ret = SysCall2(Z_ADDRESS_SPACE_MAP, &req, &resp);
  *vaddr = resp.vaddr;
  return ret;
}
uint64_t ZMemoryObjectCreate(uint64_t size, uint64_t* vmmo_cap) {
  ZMemoryObjectCreateReq req{
      .size = size,
  };
  ZMemoryObjectCreateResp resp;
  uint64_t ret = SysCall2(Z_MEMORY_OBJECT_CREATE, &req, &resp);
  *vmmo_cap = resp.vmmo_cap;
  return ret;
}

uint64_t ZDebug(const char* message) {
  return SysCall1(Z_DEBUG_PRINT, message);
}
