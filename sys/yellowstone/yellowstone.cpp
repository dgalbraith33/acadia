#include <denali/denali.h>
#include <mammoth/channel.h>
#include <mammoth/debug.h>
#include <mammoth/process.h>
#include <zcall.h>

#include "hw/pcie.h"

uint64_t main(uint64_t port_cap) {
  dbgln("Yellowstone Initializing.");
  uint64_t msg_type, type, cap, bytes, caps;
  check(ZPortRecv(port_cap, 8, reinterpret_cast<uint8_t*>(&msg_type), 1, &cap,
                  &type, &bytes, &caps));
  uint64_t vmmo_cap = 0;
  if (bytes != 8 || caps != 1) {
    crash("Invalid boot msg", Z_ERR_INVALID);
  }
  if (msg_type == Z_INIT_BOOT_VMMO) {
    vmmo_cap = cap;
  } else {
    crash("Missing vmmo cap", Z_ERR_UNIMPLEMENTED);
  }
  uint64_t vaddr;
  check(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, vmmo_cap, &vaddr));

  Channel local;
  check(SpawnProcessFromElfRegion(vaddr, local));

  DenaliRead read{
      .device_id = 0,
      .lba = 0,
      .size = 1,
  };
  check(ZChannelSend(local.cap(), DENALI_READ, sizeof(DenaliRead),
                     reinterpret_cast<uint8_t*>(&read), 0, nullptr));

  DenaliReadResponse resp;
  uint64_t mem_cap;

  check(ZChannelRecv(local.cap(), sizeof(resp),
                     reinterpret_cast<uint8_t*>(&resp), 1, &mem_cap, &type,
                     &bytes, &caps));

  dbgln("Resp: %u", type);

  check(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, mem_cap, &vaddr));
  uint32_t* mbr = reinterpret_cast<uint32_t*>(vaddr + 0x1FE);
  dbgln("MBR: %x", *mbr);

  DumpPciEDevices();

  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
