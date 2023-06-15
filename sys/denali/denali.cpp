#include <mammoth/channel.h>
#include <mammoth/debug.h>
#include <stdint.h>

#include "ahci/ahci_driver.h"
#include "denali_server.h"

int main(uint64_t bootstrap_cap) {
  AhciDriver driver;
  RET_ERR(driver.Init());

  DenaliServer server(bootstrap_cap, driver);
  RET_ERR(server.RunServer());
  // FIXME: Add thread join.
  return 0;
}
