#include "object/port.h"

#include "scheduler/scheduler.h"

Port::Port() {}

z_err_t Port::Write(const ZMessage& msg) {
  if (msg.num_bytes > 0x1000) {
    dbgln("Large message size unimplemented: %x", msg.num_bytes);
    return Z_ERR_INVALID;
  }
  dbgln("port write");

  auto message = MakeShared<Message>();
  message->type = msg.type, message->num_bytes = msg.num_bytes;
  message->bytes = new uint8_t[msg.num_bytes];
  for (uint64_t i = 0; i < msg.num_bytes; i++) {
    message->bytes[i] = msg.bytes[i];
  }

  for (uint64_t i = 0; i < msg.num_caps; i++) {
    auto cap = gScheduler->CurrentProcess().ReleaseCapability(msg.caps[i]);
    if (!cap) {
      return Z_ERR_CAP_NOT_FOUND;
    }
    message->caps.PushBack(cap);
  }

  MutexHolder lock(mutex_);
  pending_messages_.PushBack(message);
  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
  return Z_OK;
}

z_err_t Port::Read(ZMessage& msg) {
  dbgln("port read");
  mutex_.Lock();
  while (pending_messages_.size() < 1) {
    blocked_threads_.PushBack(gScheduler->CurrentThread());
    mutex_.Unlock();
    gScheduler->Yield();
    mutex_.Lock();
  }
  mutex_.Unlock();

  MutexHolder lock(mutex_);
  auto next_msg = pending_messages_.PeekFront();
  if (next_msg->num_bytes > msg.num_bytes) {
    return Z_ERR_BUFF_SIZE;
  }
  if (next_msg->caps.size() > msg.num_caps) {
    return Z_ERR_BUFF_SIZE;
  }

  msg.type = next_msg->type;
  msg.num_bytes = next_msg->num_bytes;

  for (uint64_t i = 0; i < msg.num_bytes; i++) {
    msg.bytes[i] = next_msg->bytes[i];
  }

  msg.num_caps = next_msg->caps.size();
  auto& proc = gScheduler->CurrentProcess();
  for (uint64_t i = 0; i < msg.num_caps; i++) {
    msg.caps[i] = proc.AddExistingCapability(next_msg->caps.PopFront());
  }

  pending_messages_.PopFront();

  return Z_OK;
}

bool Port::HasMessages() {
  MutexHolder h(mutex_);
  return pending_messages_.size() != 0;
}
