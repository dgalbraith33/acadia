#pragma once

#include "capability/capability.h"
#include "include/ztypes.h"
#include "lib/linked_list.h"
#include "lib/message_queue.h"
#include "lib/mutex.h"
#include "lib/pair.h"
#include "lib/ref_ptr.h"
#include "object/kernel_object.h"
#include "usr/zcall_internal.h"

class Channel;

template <>
struct KernelObjectTag<Channel> {
  static const uint64_t type = KernelObject::CHANNEL;
};

class Channel : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::CHANNEL; }
  static Pair<RefPtr<Channel>, RefPtr<Channel>> CreateChannelPair();

  RefPtr<Channel> peer() { return peer_; }

  z_err_t Write(uint64_t num_bytes, const void* bytes, uint64_t num_caps,
                const z_cap_t* caps);
  z_err_t Read(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
               z_cap_t* caps);

 private:
  // FIXME: We will likely never close the channel based on this
  // circular dependency.
  RefPtr<Channel> peer_{nullptr};

  Mutex mutex_{"channel"};
  UnboundedMessageQueue message_queue_;

  LinkedList<RefPtr<Thread>> blocked_threads_;

  friend class MakeRefCountedFriend<Channel>;
  Channel() {}
  void SetPeer(const RefPtr<Channel>& peer) { peer_ = peer; }

  z_err_t WriteInternal(uint64_t num_bytes, const void* bytes,
                        uint64_t num_caps, const z_cap_t* caps);
};
