#include "object/process.h"

#include "debug/debug.h"
#include "include/zcall.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"
#include "object/thread.h"
#include "scheduler/process_manager.h"
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
  if (tid >= threads_.size()) {
    panic("Bad thread access {} on process {} with {} threads.", tid, id_,
          threads_.size());
  }
  return threads_[tid];
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

void Process::Exit(uint64_t exit_code) {
  // TODO: Check this state elsewhere to ensure that we don't for instance
  // create a running thread on a finished process.
  state_ = CLEANUP;
  exit_code_ = exit_code;

  for (const auto& t : threads_) {
    if (!t->IsDying()) {
      t->SetState(Thread::CLEANUP);
    }
  }

  gProcMan->CleanupProcess(id_);

  // Technically we may get interrupted here the cleanup process may start,
  // truthfully that is fine. Once each thread is flagged for cleanup then it
  // will no longer be scheduled again or need to be.

  if (gScheduler->CurrentProcess().id_ == id_) {
    gScheduler->Yield();
  }
}

void Process::Cleanup() {
  if (gScheduler->CurrentProcess().id_ == id_) {
    panic("Can't clean up process from itself.");
  }
  if (state_ != CLEANUP) {
    dbgln("WARN: Cleaning up process with non-cleanup state {}",
          (uint64_t)state_);
    state_ = CLEANUP;
  }

  // 1. For each thread, call cleanup.
  for (const auto& t : threads_) {
    if (t->GetState() == Thread::CLEANUP) {
      t->Cleanup();
    }
  }

  // 2. Release all capabailities.
  caps_.ReleaseAll();

  // 3. Unmap all user memory.
  PANIC_ON_ERR(
      vmas_->FreeAddressRange(0, kUserSpaceMax, /* is_dying_proc= */ true),
      "Failed to cleanup userspace mappings in process exit.");

  // 4. Release paging structures.
  vmas_ = nullptr;

  state_ = FINISHED;
}
