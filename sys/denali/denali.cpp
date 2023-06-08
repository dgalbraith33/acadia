#include <stdint.h>

#include "ahci/ahci_driver.h"

int main(uint64_t bootstrap_cap) {
  AhciDriver driver;
  return driver.Init();
}
