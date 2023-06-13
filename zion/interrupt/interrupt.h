#pragma once

#include "lib/ref_ptr.h"
#include "object/port.h"

void InitIdt();

void RegisterPciPort(const RefPtr<Port>& port);
