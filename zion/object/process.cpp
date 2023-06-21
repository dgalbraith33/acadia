#include "object/process.h"

#include "debug/debug.h"
#include "include/zcall.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"
#include "object/thread.h"
#include "scheduler/scheduler.h"

namespace {

static uint64_t gNextId = 1;

}

glcr::RefPtr<Process> Process::RootProcess() {
  glcr::RefPtr<Process> proc = glcr::MakeRefCounted<Process>(0);
  proc->threads_.PushBack(Thread::RootThread(*proc));
  proc->next_thread_id_ = 1;

  return proc;
}
glcr::RefPtr<Process> Process::Create() {
  return glcr::MakeRefCounted<Process>();
}

Process::Process()
    : id_(gNextId++),
      vmas_(glcr::MakeRefCounted<AddressSpace>()),
      state_(RUNNING) {}

glcr::RefPtr<Thread> Process::CreateThread() {
  MutexHolder lock(mutex_);
  glcr::RefPtr<Thread> thread =
      glcr::MakeRefCounted<Thread>(*this, next_thread_id_++);
  threads_.PushBack(thread);
  return thread;
}

glcr::RefPtr<Thread> Process::GetThread(uint64_t tid) {
  MutexHolder lock(mutex_);
  auto iter = threads_.begin();
  while (iter != threads_.end()) {
    if ((*iter)->tid() == tid) {
      return *iter;
    }
    ++iter;
  }
  panic("Bad thread access.");
  return nullptr;
}

void Process::CheckState() {
  MutexHolder lock(mutex_);
  auto iter = threads_.begin();
  while (iter != threads_.end()) {
    if ((*iter)->GetState() != Thread::FINISHED) {
      return;
    }
    ++iter;
  }
  state_ = FINISHED;
}

glcr::RefPtr<Capability> Process::ReleaseCapability(uint64_t cid) {
  return caps_.ReleaseCapability(cid);
}

glcr::RefPtr<Capability> Process::GetCapability(uint64_t cid) {
  return caps_.GetCapability(cid);
}

uint64_t Process::AddExistingCapability(const glcr::RefPtr<Capability>& cap) {
  return caps_.AddExistingCapability(cap);
}
