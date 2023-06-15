#pragma once

#include "capability/capability.h"
#include "include/zerrors.h"
#include "lib/linked_list.h"
#include "lib/mutex.h"
#include "lib/pair.h"
#include "lib/ref_ptr.h"
#include "lib/shared_ptr.h"
#include "object/kernel_object.h"
#include "usr/zcall_internal.h"

class Channel : public KernelObject {
 public:
  static Pair<RefPtr<Channel>, RefPtr<Channel>> CreateChannelPair();

  RefPtr<Channel> peer() { return peer_; }

  z_err_t Write(const ZMessage& msg);
  z_err_t Read(ZMessage& msg);

 private:
  // FIXME: We will likely never close the channel based on this
  // circular dependency.
  RefPtr<Channel> peer_{nullptr};

  Mutex mutex_{"channel"};

  struct Message {
    uint64_t type;

    uint64_t num_bytes;
    uint8_t* bytes;

    LinkedList<RefPtr<Capability>> caps;
  };

  // FIXME: This is probably dangerous because of an
  // implicit shallow copy.
  LinkedList<SharedPtr<Message>> pending_messages_;
  LinkedList<RefPtr<Thread>> blocked_threads_;

  friend class MakeRefCountedFriend<Channel>;
  Channel() {}
  void SetPeer(const RefPtr<Channel>& peer) { peer_ = peer; }

  z_err_t EnqueueMessage(const ZMessage& msg);
};
