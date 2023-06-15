#pragma once

#include <stdint.h>

#include "mammoth/channel.h"

uint64_t SpawnProcessFromElfRegion(uint64_t program, Channel& local);
