#pragma once

#include "status/error.h"
#include "string/string.h"

namespace glcr {

class Status {
 public:
  static Status Ok() { return {}; }
  Status(ErrorCode code) : code_(code), message_() {}
  Status(ErrorCode code, StringView message) : code_(code), message_(message) {}

  explicit operator bool() { return ok(); }
  bool ok() { return code_ == OK; }

  ErrorCode code() { return code_; }
  StringView message() { return message_; }

 private:
  ErrorCode code_;
  String message_;

  Status();
};

Status InvalidArgument(StringView message) {
  return Status(INVALID_ARGUMENT, message);
}

Status NotFound(StringView message) { return Status(NOT_FOUND, message); }

Status PermissionDenied(StringView message) {
  return Status(PERMISSION_DENIED, message);
}

Status NullPtr(StringView message) { return Status(NULL_PTR, message); }

Status Empty(StringView message) { return Status(EMPTY, message); }

Status AlreadyExists(StringView message) {
  return Status(ALREADY_EXISTS, message);
}

Status BufferSize(StringView message) { return Status(BUFFER_SIZE, message); }

Status FailedPrecondition(StringView message) {
  return Status(FAILED_PRECONDITION, message);
}

Status Internal(StringView message) { return Status(INTERNAL, message); }

Status Unimplemented(StringView message) {
  return Status(UNIMPLEMENTED, message);
}

Status Exhausted(StringView message) { return Status(EXHAUSTED, message); }

Status InvalidResponse(StringView message) {
  return Status(INVALID_RESPONSE, message);
}

}  // namespace glcr
