#include <denali/denali.h>
#include <mammoth/channel.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <mammoth/process.h>
#include <zcall.h>

#include "hw/gpt.h"
#include "hw/pcie.h"

uint64_t main(uint64_t port_cap) {
  dbgln("Yellowstone Initializing.");
  check(ParseInitPort(port_cap));

  DumpPciEDevices();

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, gBootDenaliVmmoCap, &vaddr));

  Channel local;
  check(SpawnProcessFromElfRegion(vaddr, local));

  DenaliRead read{
      .device_id = 0,
      .lba = 0,
      .size = 1,
  };
  check(local.WriteStruct(&read));
  DenaliReadResponse resp;
  uint64_t mem_cap;
  check(local.ReadStructAndCap(&resp, &mem_cap));
  check(CheckMbrIsGpt(mem_cap));

  DenaliRead read_lba1{
      .device_id = 0,
      .lba = 1,
      .size = 1,
  };
  check(local.WriteStruct(&read_lba1));
  check(local.ReadStructAndCap(&resp, &mem_cap));
  check(ReadPartitionHeader(mem_cap));

  DenaliRead read_parts{
      .device_id = 0,
      .lba = 2,
      .size = 10,  // FIXME: Dont hardcode this.
  };
  check(local.WriteStruct(&read_parts));
  check(local.ReadStructAndCap(&resp, &mem_cap));
  check(ReadPartitionEntries(mem_cap));

  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
