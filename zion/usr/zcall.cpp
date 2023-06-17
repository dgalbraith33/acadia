#include "include/zcall.h"

#include <stdint.h>

#include "usr/zcall_internal.h"

z_err_t SysCall2(uint64_t number, const void* first, const void* second) {
  z_err_t return_code;
  asm("syscall"
      : "=a"(return_code)
      : "D"(number), "S"(first), "d"(second)
      : "rcx", "r11");
  return return_code;
}

z_err_t SysCall0(uint64_t number) { return SysCall2(number, 0, 0); }

z_err_t SysCall1(uint64_t number, const void* first) {
  return SysCall2(number, first, 0);
}

void ZProcessExit(uint64_t code) {
  SysCall1(Z_PROCESS_EXIT, reinterpret_cast<void*>(code));
}

z_err_t ZProcessSpawn(uint64_t proc_cap, uint64_t bootstrap_cap,
                      uint64_t* new_proc_cap, uint64_t* new_vmas_cap,
                      uint64_t* new_bootstrap_cap) {
  ZProcessSpawnReq req{
      .proc_cap = proc_cap,
      .bootstrap_cap = bootstrap_cap,
  };
  ZProcessSpawnResp resp;
  z_err_t ret = SysCall2(Z_PROCESS_SPAWN, &req, &resp);
  *new_proc_cap = resp.proc_cap;
  *new_vmas_cap = resp.vmas_cap;
  *new_bootstrap_cap = resp.bootstrap_cap;
  return ret;
}

z_err_t ZThreadCreate(uint64_t proc_cap, uint64_t* thread_cap) {
  ZThreadCreateReq req{
      .proc_cap = proc_cap,
  };
  ZThreadCreateResp resp;
  z_err_t ret = SysCall2(Z_THREAD_CREATE, &req, &resp);
  *thread_cap = resp.thread_cap;
  return ret;
}

z_err_t ZThreadStart(uint64_t thread_cap, uint64_t entry, uint64_t arg1,
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

z_err_t ZAddressSpaceMap(uint64_t vmas_cap, uint64_t vmas_offset,
                         uint64_t vmmo_cap, uint64_t* vaddr) {
  ZAddressSpaceMapReq req{
      .vmas_cap = vmas_cap,
      .vmas_offset = vmas_offset,
      .vmmo_cap = vmmo_cap,
  };
  ZAddressSpaceMapResp resp;
  z_err_t ret = SysCall2(Z_ADDRESS_SPACE_MAP, &req, &resp);
  *vaddr = resp.vaddr;
  return ret;
}

z_err_t ZMemoryObjectCreate(uint64_t size, uint64_t* vmmo_cap) {
  ZMemoryObjectCreateReq req{
      .size = size,
  };
  ZMemoryObjectCreateResp resp;
  z_err_t ret = SysCall2(Z_MEMORY_OBJECT_CREATE, &req, &resp);
  *vmmo_cap = resp.vmmo_cap;
  return ret;
}

z_err_t ZMemoryObjectCreatePhysical(uint64_t paddr, uint64_t size,
                                    uint64_t* vmmo_cap) {
  ZMemoryObjectCreatePhysicalReq req{
      .paddr = paddr,
      .size = size,
  };
  ZMemoryObjectCreatePhysicalResp resp;
  z_err_t ret = SysCall2(Z_MEMORY_OBJECT_CREATE_PHYSICAL, &req, &resp);
  *vmmo_cap = resp.vmmo_cap;
  return ret;
}

z_err_t ZMemoryObjectCreateContiguous(uint64_t size, uint64_t* vmmo_cap,
                                      uint64_t* paddr) {
  ZMemoryObjectCreatePhysicalReq req{
      .paddr = 0,
      .size = size,
  };
  ZMemoryObjectCreatePhysicalResp resp;
  z_err_t ret = SysCall2(Z_MEMORY_OBJECT_CREATE_PHYSICAL, &req, &resp);
  *vmmo_cap = resp.vmmo_cap;
  *paddr = resp.paddr;
  return ret;
}

z_err_t ZTempPcieConfigObjectCreate(uint64_t* vmmo_cap, uint64_t* vmmo_size) {
  ZTempPcieConfigObjectCreateResp resp;
  z_err_t ret = SysCall2(Z_TEMP_PCIE_CONFIG_OBJECT_CREATE, 0, &resp);
  *vmmo_cap = resp.vmmo_cap;
  *vmmo_size = resp.vmmo_size;
  return ret;
}

z_err_t ZChannelCreate(uint64_t* channel1, uint64_t* channel2) {
  ZChannelCreateResp resp;
  z_err_t ret = SysCall2(Z_CHANNEL_CREATE, 0, &resp);
  *channel1 = resp.chan_cap1;
  *channel2 = resp.chan_cap2;
  return ret;
}

z_err_t ZChannelSend(uint64_t chan_cap, uint64_t type, uint64_t num_bytes,
                     const uint8_t* bytes, uint64_t num_caps,
                     const uint64_t* caps) {
  ZChannelSendReq req{
      .chan_cap = chan_cap,
      .message =
          {
              .type = type,
              .num_bytes = num_bytes,
              .bytes = const_cast<uint8_t*>(bytes),
              .num_caps = num_caps,
              .caps = const_cast<uint64_t*>(caps),
          },
  };
  return SysCall1(Z_CHANNEL_SEND, &req);
}

z_err_t ZChannelRecv(uint64_t chan_cap, uint64_t num_bytes, uint8_t* bytes,
                     uint64_t num_caps, uint64_t* caps, uint64_t* type,
                     uint64_t* actual_bytes, uint64_t* actual_caps) {
  ZChannelRecvReq req{
      .chan_cap = chan_cap,
      .message =
          {
              .type = 0,
              .num_bytes = num_bytes,
              .bytes = bytes,
              .num_caps = num_caps,
              .caps = caps,
          },
  };
  z_err_t ret = SysCall1(Z_CHANNEL_RECV, &req);
  *type = req.message.type;
  *actual_bytes = req.message.num_bytes;
  *actual_caps = req.message.num_caps;
  return ret;
}

z_err_t ZPortRecv(uint64_t port_cap, uint64_t num_bytes, uint8_t* bytes,
                  uint64_t num_caps, uint64_t* caps, uint64_t* type,
                  uint64_t* actual_bytes, uint64_t* actual_caps) {
  ZPortRecvReq req{
      .port_cap = port_cap,
      .message =
          {
              .type = 0,
              .num_bytes = num_bytes,
              .bytes = bytes,
              .num_caps = num_caps,
              .caps = caps,
          },
  };
  z_err_t ret = SysCall1(Z_PORT_RECV, &req);
  *type = req.message.type;
  *actual_bytes = req.message.num_bytes;
  *actual_caps = req.message.num_caps;
  return ret;
}

z_err_t ZPortPoll(uint64_t port_cap, uint64_t num_bytes, uint8_t* bytes,
                  uint64_t num_caps, uint64_t* caps, uint64_t* type,
                  uint64_t* actual_bytes, uint64_t* actual_caps) {
  ZPortRecvReq req{
      .port_cap = port_cap,
      .message =
          {
              .type = 0,
              .num_bytes = num_bytes,
              .bytes = bytes,
              .num_caps = num_caps,
              .caps = caps,
          },
  };
  z_err_t ret = SysCall1(Z_PORT_POLL, &req);
  *type = req.message.type;
  *actual_bytes = req.message.num_bytes;
  *actual_caps = req.message.num_caps;
  return ret;
}

z_err_t ZIrqRegister(uint64_t irq_num, uint64_t* port_cap) {
  ZIrqRegisterReq req{
      .irq_num = irq_num,
  };
  ZIrqRegisterResp resp;
  z_err_t ret = SysCall2(Z_IRQ_REGISTER, &req, &resp);
  *port_cap = resp.port_cap;
  return ret;
}

z_err_t ZDebug(const char* message) { return SysCall1(Z_DEBUG_PRINT, message); }
