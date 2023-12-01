#pragma once

#include <stdint.h>
#include <ztypes.h>

extern uint64_t gSelfProcCap;
extern uint64_t gSelfVmasCap;

extern uint64_t gInitEndpointCap;

extern uint64_t gBootDenaliVmmoCap;
extern uint64_t gBootVictoriaFallsVmmoCap;
extern uint64_t gBootPciVmmoCap;
extern uint64_t gBootFramebufferVmmoCap;

z_err_t ParseInitPort(uint64_t init_port_cap);
