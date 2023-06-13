#include <mammoth/debug.h>
#include <stdint.h>

#include "ahci/ahci_driver.h"

int main(uint64_t bootstrap_cap) {
  AhciDriver driver;
  RET_ERR(driver.Init());

  while (1) {
  };
  return 0;
}
