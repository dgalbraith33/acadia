#pragma once

#include <glacier/status/error.h>
#include <stdint.h>

#include "mammoth/endpoint_client.h"

glcr::ErrorCode SpawnProcessFromElfRegion(uint64_t program,
                                          EndpointClient client);
