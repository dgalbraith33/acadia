#pragma once

#include <glacier/memory/ref_ptr.h>

#include "object/port.h"

void InitIdt();

void UpdateFaultHandlersToIst1();

void RegisterPciPort(const glcr::RefPtr<Port>& port);
