#include "object/channel.h"

#include "include/zerrors.h"

Pair<RefPtr<Channel>, RefPtr<Channel>> Channel::CreateChannelPair() {
  auto c1 = MakeRefCounted<Channel>();
  auto c2 = MakeRefCounted<Channel>();
  c1->SetPeer(c2);
  c2->SetPeer(c1);
  return {c1, c2};
}

z_err_t Channel::Write(const ZMessage& msg) {
  return peer_->EnqueueMessage(msg);
}

z_err_t Channel::Read(ZMessage& msg) {
  if (pending_messages_.size() == 0) {
    dbgln("Unimplemented add blocking.");
    return Z_ERR_UNIMPLEMENTED;
  }
  Message next_msg = pending_messages_.PeekFront();
  if (next_msg.num_bytes > msg.num_bytes) {
    return Z_ERR_BUFF_SIZE;
  }

  msg.type = next_msg.type;
  msg.num_bytes = next_msg.num_bytes;
  msg.num_caps = 0;

  for (uint64_t i = 0; i < msg.num_bytes; i++) {
    msg.bytes[i] = next_msg.bytes[i];
  }

  pending_messages_.PopFront();

  return Z_OK;
}

z_err_t Channel::EnqueueMessage(const ZMessage& msg) {
  if (msg.num_caps > 0) {
    dbgln("Unimplemented passing caps on channel");
    return Z_ERR_UNIMPLEMENTED;
  }

  if (msg.num_bytes > 0x1000) {
    dbgln("Large message size unimplemented: %x", msg.num_bytes);
    return Z_ERR_INVALID;
  }

  Message message{
      .type = msg.type,
      .num_bytes = msg.num_bytes,
      .bytes = new uint8_t[msg.num_bytes],
  };
  for (uint64_t i = 0; i < msg.num_bytes; i++) {
    message.bytes[i] = msg.bytes[i];
  }
  pending_messages_.PushBack(message);
  return Z_OK;
}
