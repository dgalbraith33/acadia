#pragma once

#include <stdint.h>

#include "glacier/string/string_view.h"

namespace glcr {

enum ErrorCode : uint64_t {
  OK = 0x0,
  // First set of error codes generally indicate user errors.
  INVALID_ARGUMENT = 0x1,
  NOT_FOUND = 0x2,
  PERMISSION_DENIED = 0x3,
  NULL_PTR = 0x4,
  EMPTY = 0x5,
  ALREADY_EXISTS = 0x6,
  BUFFER_SIZE = 0x7,
  FAILED_PRECONDITION = 0x8,

  // Second set of error codes generally indicate service errors.
  INTERNAL = 0x100,
  UNIMPLEMENTED = 0x101,
  EXHAUSTED = 0x102,
  INVALID_RESPONSE = 0x103,

  // Kernel specific error codes (relating to capabilities).
  CAP_NOT_FOUND = 0x1000,
  CAP_WRONG_TYPE = 0x1001,
  CAP_PERMISSION_DENIED = 0x1002,

};

StringView ErrorCodeToStr(ErrorCode code);

#define RET_ERR(expr)                                              \
  {                                                                \
    glcr::ErrorCode _tmp_err = static_cast<glcr::ErrorCode>(expr); \
    if (_tmp_err != glcr::OK) {                                    \
      return _tmp_err;                                             \
    }                                                              \
  }

}  // namespace glcr
