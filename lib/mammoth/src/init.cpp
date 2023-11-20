#include "mammoth/init.h"

#include <glacier/status/error.h>
#include <ztypes.h>

#include "mammoth/debug.h"
#include "mammoth/port_server.h"

uint64_t gSelfProcCap = 0;
uint64_t gSelfVmasCap = 0;

uint64_t gInitEndpointCap = 0;

uint64_t gBootDenaliVmmoCap = 0;
uint64_t gBootVictoriaFallsVmmoCap = 0;
uint64_t gBootPciVmmoCap = 0;
uint64_t gBootFramebufferVmmoCap = 0;

z_err_t ParseInitPort(uint64_t init_port_cap) {
  PortServer port = PortServer::AdoptCap(init_port_cap);
  z_err_t ret;
  uint64_t init_sig, init_cap;
  while ((ret = port.PollForIntCap(&init_sig, &init_cap)) != glcr::EMPTY) {
    RET_ERR(ret);
    switch (init_sig) {
      case Z_INIT_SELF_PROC:
        gSelfProcCap = init_cap;
        break;
      case Z_INIT_SELF_VMAS:
        gSelfVmasCap = init_cap;
        break;
      case Z_INIT_ENDPOINT:
        gInitEndpointCap = init_cap;
        break;
      case Z_BOOT_DENALI_VMMO:
        gBootDenaliVmmoCap = init_cap;
        break;
      case Z_BOOT_VICTORIA_FALLS_VMMO:
        gBootVictoriaFallsVmmoCap = init_cap;
        break;
      case Z_BOOT_PCI_VMMO:
        gBootPciVmmoCap = init_cap;
        break;
      case Z_BOOT_FRAMEBUFFER_INFO_VMMO:
        gBootFramebufferVmmoCap = init_cap;
        break;
      default:
        dbgln("Unexpected init type {x}, continuing.", init_sig);
    }
  }

  return glcr::OK;
}
