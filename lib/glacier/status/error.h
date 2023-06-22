#pragma once

#include <stdint.h>

namespace glcr {

typedef uint64_t ErrorCode;

#define RET_ERR(expr)           \
  {                             \
    z_err_t _tmp_err = expr;    \
    if (_tmp_err != glcr::OK) { \
      return _tmp_err;          \
    }                           \
  }

static const uint64_t OK = 0x0;

// First set of error codes generally indicate user errors.
static const uint64_t INVALID_ARGUMENT = 0x1;
static const uint64_t NOT_FOUND = 0x2;
static const uint64_t PERMISSION_DENIED = 0x3;
static const uint64_t NULL_PTR = 0x4;
static const uint64_t EMPTY = 0x5;
static const uint64_t ALREADY_EXISTS = 0x6;
static const uint64_t BUFFER_SIZE = 0x7;
static const uint64_t FAILED_PRECONDITION = 0x8;

// Second set of error codes generally indicate service errors.
static const uint64_t INTERNAL = 0x100;
static const uint64_t UNIMPLEMENTED = 0x101;
static const uint64_t EXHAUSTED = 0x102;

// Kernel specific error codes (relating to capabilities).
static const uint64_t CAP_NOT_FOUND = 0x1000;
static const uint64_t CAP_WRONG_TYPE = 0x1001;
static const uint64_t CAP_PERMISSION_DENIED = 0x1002;

}  // namespace glcr
