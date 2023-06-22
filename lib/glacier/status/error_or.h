#pragma once

#include <glacier/status/error.h>

namespace glcr {

template <typename T>
class ErrorOr {
 public:
  ErrorOr() = delete;
  ErrorOr(const ErrorOr&) = delete;
  ErrorOr(ErrorOr&&) = delete;
  // FIXME: Do we have to call ~T manually here.
  ~ErrorOr() {}

  ErrorOr(ErrorCode code) : error_(code), ok_(false) {}
  ErrorOr(const T& obj) : obj_(obj), ok_(true) {}
  ErrorOr(T&& obj) : obj_(obj), ok_(true) {}

  bool ok() { return ok_; }
  operator bool() { return ok_; }

  T& value() { return obj_; }

  ErrorCode error() { return error_; }

 private:
  union {
    ErrorCode error_;
    T obj_;
  };
  bool ok_;
};

#define AOR_INNER(a, b) a##b
#define AOR_VAR(a) AOR_INNER(e, a)

#define ASSIGN_OR_RETURN(lhs, rhs)    \
                                      \
  auto AOR_VAR(__LINE__) = rhs;       \
  if (!AOR_VAR(__LINE__).ok()) {      \
    return AOR_VAR(__LINE__).error(); \
  }                                   \
  lhs = AOR_VAR(__LINE__).value();

}  // namespace glcr
