#pragma once

#include <stdint.h>

#define Z_OK 0x0
#define Z_ERR_NOT_FOUND 0x1
#define Z_ERR_INVALID 0x2
#define Z_ERR_DENIED 0x3
#define Z_ERR_UNIMPLEMENTED 0x4
#define Z_ERR_BUFF_SIZE 0x05

#define Z_ERR_CAP_NOT_FOUND 0x100
#define Z_ERR_CAP_TYPE 0x101
#define Z_ERR_CAP_DENIED 0x102

typedef uint64_t z_err_t;
