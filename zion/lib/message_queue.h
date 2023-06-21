#pragma once

#include "capability/capability.h"
#include "glacier/memory/shared_ptr.h"
#include "include/ztypes.h"
#include "lib/linked_list.h"

class MessageQueue {
 public:
  virtual ~MessageQueue() {}

  virtual z_err_t PushBack(uint64_t num_bytes, const void* bytes,
                           uint64_t num_caps, const z_cap_t* caps) = 0;
  virtual z_err_t PopFront(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
                           z_cap_t* caps) = 0;
};

class UnboundedMessageQueue : public MessageQueue {
 public:
  UnboundedMessageQueue() {}
  UnboundedMessageQueue(const UnboundedMessageQueue&) = delete;
  UnboundedMessageQueue& operator=(const UnboundedMessageQueue&) = delete;
  virtual ~UnboundedMessageQueue() override {}

  z_err_t PushBack(uint64_t num_bytes, const void* bytes, uint64_t num_caps,
                   const z_cap_t* caps) override;
  z_err_t PopFront(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
                   z_cap_t* caps) override;

  void WriteKernel(uint64_t init, RefPtr<Capability> cap);

  uint64_t size() { return pending_messages_.size(); }
  bool empty() { return size() == 0; }

 private:
  struct Message {
    uint64_t num_bytes;
    uint8_t* bytes;

    LinkedList<RefPtr<Capability>> caps;
  };

  LinkedList<glcr::SharedPtr<Message>> pending_messages_;
};
