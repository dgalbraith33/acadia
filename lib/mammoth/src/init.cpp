#include "mammoth/init.h"

#include <ztypes.h>

#include "mammoth/debug.h"
#include "mammoth/port.h"

uint64_t gSelfProcCap = 0;
uint64_t gSelfVmasCap = 0;

uint64_t gInitChannelCap = 0;

uint64_t gBootDenaliVmmoCap = 0;

z_err_t ParseInitPort(uint64_t init_port_cap) {
  Port port(init_port_cap);
  z_err_t ret;
  uint64_t init_sig, init_cap;
  while ((ret = port.PollForIntCap(&init_sig, &init_cap)) != Z_ERR_EMPTY) {
    RET_ERR(ret);
    switch (init_sig) {
      case Z_INIT_SELF_PROC:
        dbgln("received proc");
        gSelfProcCap = init_cap;
        break;
      case Z_INIT_SELF_VMAS:
        dbgln("received vmas");
        gSelfVmasCap = init_cap;
        break;
      case Z_INIT_CHANNEL:
        gInitChannelCap = init_cap;
        break;
      case Z_BOOT_DENALI_VMMO:
        dbgln("received denali");
        gBootDenaliVmmoCap = init_cap;
        break;
      default:
        dbgln("Unexpected init type %x, continuing.", init_sig);
    }
  }

  return Z_OK;
}
