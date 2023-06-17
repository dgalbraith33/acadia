#include <mammoth/channel.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <stdint.h>

#include "ahci/ahci_driver.h"
#include "denali_server.h"

uint64_t main(uint64_t init_port_cap) {
  check(ParseInitPort(init_port_cap));
  AhciDriver driver;
  RET_ERR(driver.Init());

  DenaliServer server(gInitChannelCap, driver);
  RET_ERR(server.RunServer());
  // FIXME: Add thread join.
  return 0;
}
