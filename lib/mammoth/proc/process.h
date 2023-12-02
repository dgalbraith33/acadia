#pragma once

#include <glacier/status/error_or.h>
#include <stdint.h>
#include <ztypes.h>

namespace mmth {

glcr::ErrorOr<z_cap_t> SpawnProcessFromElfRegion(uint64_t program,
                                                 z_cap_t yellowstone_client);

}  // namespace mmth
