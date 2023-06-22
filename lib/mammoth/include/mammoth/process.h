#pragma once

#include <glacier/status/error_or.h>
#include <stdint.h>

#include "mammoth/endpoint_client.h"

glcr::ErrorOr<EndpointClient> SpawnProcessFromElfRegion(uint64_t program);
