#pragma once

#include <glacier/status/error.h>
#include <stdint.h>
#include <ztypes.h>

namespace mmth {

glcr::ErrorCode SpawnProcessFromElfRegion(uint64_t program,
                                          z_cap_t yellowstone_client);

}  // namespace mmth
