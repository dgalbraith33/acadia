#pragma once

#include <glacier/memory/ref_ptr.h>
#include <glacier/memory/shared_ptr.h>
#include <glacier/status/error.h>

#include "capability/capability.h"
#include "include/ztypes.h"
#include "lib/linked_list.h"

class MessageQueue {
 public:
  virtual ~MessageQueue() {}

  virtual glcr::ErrorCode PushBack(uint64_t num_bytes, const void* bytes,
                                   uint64_t num_caps, const z_cap_t* caps) = 0;
  virtual glcr::ErrorCode PopFront(uint64_t* num_bytes, void* bytes,
                                   uint64_t* num_caps, z_cap_t* caps) = 0;
};

class UnboundedMessageQueue : public MessageQueue {
 public:
  UnboundedMessageQueue() {}
  UnboundedMessageQueue(const UnboundedMessageQueue&) = delete;
  UnboundedMessageQueue& operator=(const UnboundedMessageQueue&) = delete;
  virtual ~UnboundedMessageQueue() override {}

  glcr::ErrorCode PushBack(uint64_t num_bytes, const void* bytes,
                           uint64_t num_caps, const z_cap_t* caps) override;
  glcr::ErrorCode PopFront(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
                           z_cap_t* caps) override;

  void WriteKernel(uint64_t init, glcr::RefPtr<Capability> cap);

  uint64_t size() { return pending_messages_.size(); }
  bool empty() { return size() == 0; }

 private:
  struct Message {
    uint64_t num_bytes;
    uint8_t* bytes;

    LinkedList<glcr::RefPtr<Capability>> caps;
  };

  LinkedList<glcr::SharedPtr<Message>> pending_messages_;
};

class SingleMessageQueue : public MessageQueue {
 public:
  SingleMessageQueue() {}
  SingleMessageQueue(const SingleMessageQueue&) = delete;
  SingleMessageQueue(SingleMessageQueue&&) = delete;
  virtual ~SingleMessageQueue() override {}

  glcr::ErrorCode PushBack(uint64_t num_bytes, const void* bytes,
                           uint64_t num_caps, const z_cap_t* caps) override;
  glcr::ErrorCode PopFront(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
                           z_cap_t* caps) override;

  bool empty() { return has_written_ == false; };

 private:
  bool has_written_ = false;
  bool has_read_ = false;
  uint64_t num_bytes_;
  uint8_t* bytes_;
  LinkedList<glcr::RefPtr<Capability>> caps_;
};
