#pragma once

#include "capability/capability.h"
#include "lib/linked_list.h"
#include "lib/pair.h"
#include "lib/ref_ptr.h"
#include "object/kernel_object.h"
#include "usr/zcall_internal.h"

class Channel : public KernelObject {
 public:
  static Pair<RefPtr<Channel>, RefPtr<Channel>> CreateChannelPair();

  RefPtr<Channel> peer() { return peer_; }

  uint64_t Write(const ZMessage& msg);
  uint64_t Read(ZMessage& msg);

 private:
  // FIXME: We will likely never close the channel based on this
  // circular dependency.
  RefPtr<Channel> peer_{nullptr};

  struct Message {
    uint64_t type;

    uint64_t num_bytes;
    uint8_t* bytes;
  };

  // FIXME: This is probably dangerous because of an
  // implicit shallow copy.
  LinkedList<Message> pending_messages_;

  friend class MakeRefCountedFriend<Channel>;
  Channel() {}
  void SetPeer(const RefPtr<Channel>& peer) { peer_ = peer; }

  uint64_t EnqueueMessage(const ZMessage& msg);
};
