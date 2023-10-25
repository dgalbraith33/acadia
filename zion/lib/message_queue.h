#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/container/linked_list.h>
#include <glacier/memory/ref_ptr.h>
#include <glacier/memory/shared_ptr.h>
#include <glacier/status/error.h>

#include "capability/capability.h"
#include "include/ztypes.h"
#include "lib/mutex.h"

class MessageQueue {
 public:
  virtual ~MessageQueue() {}

  virtual glcr::ErrorCode PushBack(uint64_t num_bytes, const void* bytes,
                                   uint64_t num_caps, const z_cap_t* caps,
                                   z_cap_t reply_cap = 0) = 0;
  virtual glcr::ErrorCode PopFront(uint64_t* num_bytes, void* bytes,
                                   uint64_t* num_caps, z_cap_t* caps,
                                   z_cap_t* reply_cap = nullptr) = 0;
  virtual bool empty() = 0;

 protected:
  Mutex mutex_{"message"};
  // FIXME: This maybe shouldn't be shared between classes since the
  // SingleMessageQueue should only ever have one blocked thread.
  glcr::IntrusiveList<Thread> blocked_threads_;
};

class UnboundedMessageQueue : public MessageQueue {
 public:
  UnboundedMessageQueue() {}
  UnboundedMessageQueue(const UnboundedMessageQueue&) = delete;
  UnboundedMessageQueue& operator=(const UnboundedMessageQueue&) = delete;
  virtual ~UnboundedMessageQueue() override {}

  glcr::ErrorCode PushBack(uint64_t num_bytes, const void* bytes,
                           uint64_t num_caps, const z_cap_t* caps,
                           z_cap_t reply_cap) override;
  glcr::ErrorCode PopFront(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
                           z_cap_t* caps, z_cap_t* reply_cap) override;

  void WriteKernel(uint64_t init, glcr::RefPtr<Capability> cap);

  bool empty() override {
    MutexHolder h(mutex_);
    return pending_messages_.size() == 0;
  }

 private:
  struct Message {
    uint64_t num_bytes;
    uint8_t* bytes;

    glcr::LinkedList<glcr::RefPtr<Capability>> caps;
    glcr::RefPtr<Capability> reply_cap;
  };

  glcr::LinkedList<glcr::SharedPtr<Message>> pending_messages_;
};

class SingleMessageQueue : public MessageQueue {
 public:
  SingleMessageQueue() {}
  SingleMessageQueue(const SingleMessageQueue&) = delete;
  SingleMessageQueue(SingleMessageQueue&&) = delete;
  virtual ~SingleMessageQueue() override {}

  glcr::ErrorCode PushBack(uint64_t num_bytes, const void* bytes,
                           uint64_t num_caps, const z_cap_t* caps,
                           z_cap_t reply_cap) override;
  glcr::ErrorCode PopFront(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
                           z_cap_t* caps, z_cap_t* reply_cap) override;

  bool empty() override {
    MutexHolder h(mutex_);
    return has_written_ == false;
  };

 private:
  bool has_written_ = false;
  bool has_read_ = false;
  uint64_t num_bytes_;
  uint8_t* bytes_;
  glcr::LinkedList<glcr::RefPtr<Capability>> caps_;
};
