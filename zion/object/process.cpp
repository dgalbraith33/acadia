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

RefPtr<Process> Process::RootProcess() {
  RefPtr<Process> proc = MakeRefCounted<Process>(0);
  proc->threads_.PushBack(Thread::RootThread(*proc));
  proc->next_thread_id_ = 1;

  return proc;
}
RefPtr<Process> Process::Create() {
  auto proc = MakeRefCounted<Process>();
  proc->AddNewCapabilityWithId(Z_INIT_PROC_SELF, proc,
                               ZC_PROC_SPAWN_PROC | ZC_PROC_SPAWN_THREAD);
  proc->AddNewCapabilityWithId(Z_INIT_VMAS_SELF, proc->vmas(), ZC_WRITE);
  return proc;
}

Process::Process()
    : id_(gNextId++), vmas_(MakeRefCounted<AddressSpace>()), state_(RUNNING) {}

RefPtr<Thread> Process::CreateThread() {
  MutexHolder lock(mutex_);
  RefPtr<Thread> thread = MakeRefCounted<Thread>(*this, next_thread_id_++);
  threads_.PushBack(thread);
  return thread;
}

RefPtr<Thread> Process::GetThread(uint64_t tid) {
  MutexHolder lock(mutex_);
  auto iter = threads_.begin();
  while (iter != threads_.end()) {
    if (iter->tid() == tid) {
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
    if (iter->GetState() != Thread::FINISHED) {
      return;
    }
    ++iter;
  }
  state_ = FINISHED;
}

RefPtr<Capability> Process::ReleaseCapability(uint64_t cid) {
  return caps_.ReleaseCapability(cid);
}

RefPtr<Capability> Process::GetCapability(uint64_t cid) {
  return caps_.GetCapability(cid);
}

uint64_t Process::AddExistingCapability(const RefPtr<Capability>& cap) {
  return caps_.AddExistingCapability(cap);
}
