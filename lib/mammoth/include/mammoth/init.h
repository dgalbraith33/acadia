#pragma once

#include <stdint.h>
#include <zerrors.h>

extern uint64_t gSelfProcCap;
extern uint64_t gSelfVmasCap;

extern uint64_t gBootDenaliVmmoCap;

z_err_t ParseInitPort(uint64_t init_port_cap);
