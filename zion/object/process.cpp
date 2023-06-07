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
  proc->caps_.PushBack(
      new Capability(proc, Capability::PROCESS, Z_INIT_PROC_SELF,
                     ZC_PROC_SPAWN_PROC | ZC_PROC_SPAWN_THREAD));
  proc->caps_.PushBack(new Capability(proc->vmas(), Capability::ADDRESS_SPACE,
                                      Z_INIT_AS_SELF, ZC_WRITE));
  return proc;
}

Process::Process()
    : id_(gNextId++), vmm_(MakeRefCounted<AddressSpace>()), state_(RUNNING) {}

RefPtr<Thread> Process::CreateThread() {
  RefPtr<Thread> thread = MakeRefCounted<Thread>(*this, next_thread_id_++);
  threads_.PushBack(thread);
  return thread;
}

RefPtr<Thread> Process::GetThread(uint64_t tid) {
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
  auto iter = threads_.begin();
  while (iter != threads_.end()) {
    if (iter->GetState() != Thread::FINISHED) {
      return;
    }
    ++iter;
  }
  state_ = FINISHED;
}

SharedPtr<Capability> Process::GetCapability(uint64_t cid) {
  auto iter = caps_.begin();
  while (iter != caps_.end()) {
    if (iter->id() == cid) {
      return *iter;
    }
    ++iter;
  }
  dbgln("Bad cap access");
  dbgln("Num caps: %u", caps_.size());
  return {};
}

uint64_t Process::AddCapability(const RefPtr<Thread>& thread) {
  uint64_t cap_id = next_cap_id_++;
  caps_.PushBack(new Capability(thread, Capability::THREAD, cap_id, ZC_WRITE));
  return cap_id;
}

uint64_t Process::AddCapability(const RefPtr<Process>& p) {
  uint64_t cap_id = next_cap_id_++;
  caps_.PushBack(new Capability(p, Capability::PROCESS, cap_id,
                                ZC_WRITE | ZC_PROC_SPAWN_THREAD));
  return cap_id;
}
uint64_t Process::AddCapability(const RefPtr<AddressSpace>& as) {
  uint64_t cap_id = next_cap_id_++;
  caps_.PushBack(
      new Capability(as, Capability::ADDRESS_SPACE, cap_id, ZC_WRITE));
  return cap_id;
}
uint64_t Process::AddCapability(const RefPtr<MemoryObject>& mo) {
  uint64_t cap_id = next_cap_id_++;
  caps_.PushBack(
      new Capability(mo, Capability::MEMORY_OBJECT, cap_id, ZC_WRITE));
  return cap_id;
}
