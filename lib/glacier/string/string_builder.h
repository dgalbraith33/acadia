#pragma once

#include "glacier/container/vector.h"
#include "glacier/string/string.h"
#include "glacier/string/string_view.h"

namespace glcr {

class StringBuilder {
 public:
  virtual uint64_t size() const = 0;
  virtual void PushBack(const StringView& str) = 0;
  virtual void PushBack(const char str) = 0;

  virtual void DeleteLast() = 0;
  virtual void Reset() = 0;

  virtual String ToString() const = 0;

  virtual operator StringView() const = 0;
};

class VariableStringBuilder : public StringBuilder {
 public:
  VariableStringBuilder() = default;
  VariableStringBuilder(const VariableStringBuilder&) = delete;
  VariableStringBuilder(VariableStringBuilder&&) = default;

  ~VariableStringBuilder() = default;

  virtual uint64_t size() const override;

  virtual void PushBack(const StringView& str) override;
  virtual void PushBack(const char str) override;

  virtual void DeleteLast() override;
  virtual void Reset() override;

  virtual String ToString() const override;

  // Note that this could become invalidated
  // at any time if more characters are pushed
  // onto the builder.
  virtual operator StringView() const override;

 private:
  Vector<char> data_;
};

class FixedStringBuilder : public StringBuilder {
 public:
  FixedStringBuilder() = delete;
  FixedStringBuilder(const FixedStringBuilder&) = delete;
  FixedStringBuilder(FixedStringBuilder&&) = delete;
  ~FixedStringBuilder() = default;

  FixedStringBuilder(char* buffer, uint64_t size)
      : buffer_(buffer), capacity_(size), size_(0) {}

  virtual uint64_t size() const override { return size_; }

  virtual void PushBack(const StringView& str) override;
  virtual void PushBack(const char str) override;

  virtual void DeleteLast() override;
  virtual void Reset() override;

  virtual String ToString() const override;

  virtual operator StringView() const override;

 private:
  char* buffer_;
  uint64_t capacity_;
  uint64_t size_;
};

}  // namespace glcr
