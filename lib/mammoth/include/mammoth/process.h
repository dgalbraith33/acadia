#pragma once

#include <glacier/status/error_or.h>
#include <stdint.h>

#include "mammoth/channel.h"

glcr::ErrorOr<Channel> SpawnProcessFromElfRegion(uint64_t program);
