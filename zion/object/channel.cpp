#include "object/channel.h"

#include "include/ztypes.h"
#include "scheduler/scheduler.h"

glcr::Pair<glcr::RefPtr<Channel>, glcr::RefPtr<Channel>>
Channel::CreateChannelPair() {
  auto c1 = glcr::AdoptPtr(new Channel);
  auto c2 = glcr::AdoptPtr(new Channel);
  c1->SetPeer(c2);
  c2->SetPeer(c1);
  return {c1, c2};
}
