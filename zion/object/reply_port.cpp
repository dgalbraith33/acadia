#include "object/reply_port.h"

#include "scheduler/scheduler.h"

glcr::RefPtr<ReplyPort> ReplyPort::Create() {
  return glcr::AdoptPtr(new ReplyPort);
}
