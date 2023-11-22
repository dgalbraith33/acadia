#include "glacier/status/error.h"

namespace glcr {

StringView ErrorCodeToStr(ErrorCode code) {
  switch (code) {
    case OK:
      return "OK";
    case INVALID_ARGUMENT:
      return "INVALID_ARGUMENT";
    case NOT_FOUND:
      return "NOT_FOUND";
    case PERMISSION_DENIED:
      return "PERMISSION_DENIED";
    case NULL_PTR:
      return "NULL_PTR";
    case EMPTY:
      return "EMPTY";
    case ALREADY_EXISTS:
      return "ALREADY_EXISTS";
    case BUFFER_SIZE:
      return "BUFFER_SIZE";
    case FAILED_PRECONDITION:
      return "FAILED_PRECONDITION";
    case INTERNAL:
      return "INTERNAL";
    case UNIMPLEMENTED:
      return "UNIMPLEMENTED";
    case EXHAUSTED:
      return "EXHAUSTED";
    case INVALID_RESPONSE:
      return "INVALID_RESPONSE";
    case CAP_NOT_FOUND:
      return "CAP_NOT_FOUND";
    case CAP_WRONG_TYPE:
      return "CAP_WRONG_TYPE";
    case CAP_PERMISSION_DENIED:
      return "CAP_PERMISSION_DENIED";
    default:
      return "UNKNOWN";
  }
}

}  // namespace glcr
