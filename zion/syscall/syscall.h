#pragma once

#include "capability/capability.h"
#include "include/ztypes.h"

void InitSyscall();

// FIXME: This probably belongs in capability.h
z_err_t ValidateCap(const RefPtr<Capability>& cap, uint64_t permissions);
