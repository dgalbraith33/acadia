#pragma once

#include "glacier/status/error.h"
#include "glacier/string/string.h"

namespace glcr {

class Status {
 public:
  static Status Ok() { return {}; }
  Status(ErrorCode code) : code_(code), message_() {}
  Status(ErrorCode code, StringView message) : code_(code), message_(message) {}

  explicit operator bool() const { return ok(); }
  bool ok() const { return code_ == OK; }

  ErrorCode code() const { return code_; }
  StringView message() const { return message_; }

 private:
  ErrorCode code_;
  String message_;

  Status() : code_(OK) {}
};

inline Status InvalidArgument(StringView message) {
  return Status(INVALID_ARGUMENT, message);
}

inline Status NotFound(StringView message) {
  return Status(NOT_FOUND, message);
}

inline Status PermissionDenied(StringView message) {
  return Status(PERMISSION_DENIED, message);
}

inline Status NullPtr(StringView message) { return Status(NULL_PTR, message); }

inline Status Empty(StringView message) { return Status(EMPTY, message); }

inline Status AlreadyExists(StringView message) {
  return Status(ALREADY_EXISTS, message);
}

inline Status BufferSize(StringView message) {
  return Status(BUFFER_SIZE, message);
}

inline Status FailedPrecondition(StringView message) {
  return Status(FAILED_PRECONDITION, message);
}

inline Status Internal(StringView message) { return Status(INTERNAL, message); }

inline Status Unimplemented(StringView message) {
  return Status(UNIMPLEMENTED, message);
}

inline Status Exhausted(StringView message) {
  return Status(EXHAUSTED, message);
}

inline Status InvalidResponse(StringView message) {
  return Status(INVALID_RESPONSE, message);
}

#define RETURN_ERROR(expr)        \
  {                               \
    glcr::Status _tmp_err = expr; \
    if (!_tmp_err) {              \
      return _tmp_err;            \
    }                             \
  }

}  // namespace glcr
