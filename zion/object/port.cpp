#include "object/port.h"

Port::Port() {}

z_err_t Port::Write(const ZMessage& msg) {
  if (msg.num_caps > 0) {
    dbgln("Unimplemented passing caps on port");
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

z_err_t Port::Read(ZMessage& msg) {
  if (pending_messages_.size() < 1) {
    dbgln("Implement blocking");
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
