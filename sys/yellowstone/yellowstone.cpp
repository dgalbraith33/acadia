#include <denali/denali.h>
#include <mammoth/channel.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <mammoth/process.h>
#include <zcall.h>

#include "hw/pcie.h"

uint64_t main(uint64_t port_cap) {
  dbgln("Yellowstone Initializing.");
  check(ParseInitPort(port_cap));

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, gBootDenaliVmmoCap, &vaddr));

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
  uint64_t mem_cap, type, bytes, caps;

  check(ZChannelRecv(local.cap(), sizeof(resp),
                     reinterpret_cast<uint8_t*>(&resp), 1, &mem_cap, &type,
                     &bytes, &caps));

  dbgln("Resp: %u", type);

  check(ZAddressSpaceMap(gSelfVmasCap, 0, mem_cap, &vaddr));
  uint32_t* mbr = reinterpret_cast<uint32_t*>(vaddr + 0x1FE);
  dbgln("MBR: %x", *mbr);

  DumpPciEDevices();

  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
