#include <glacier/container/intrusive_list.h>
#include <glacier/string/str_format.h>
#include <stdint.h>
#include <zcall.h>
#include <zglobal.h>

#include "util/debug.h"

namespace {

class PageAllocator {
 public:
  static uint64_t AllocatePagePair() {
    uint64_t mem_cap;
    check(ZMemoryObjectCreate(0x2000, &mem_cap));

    uint64_t vaddr;
    check(ZAddressSpaceMap(gSelfVmasCap, 0, mem_cap, /* align= */ 0x2000,
                           &vaddr));

    // The address space mapping will keep this memory capability alive.
    check(ZCapRelease(mem_cap));

    return vaddr;
  }
};

struct BuddySlot {
  BuddySlot* next;
  BuddySlot* prev;
  uint64_t size;
};

BuddySlot* GetBuddy(BuddySlot* slot) {
  return reinterpret_cast<BuddySlot*>(reinterpret_cast<uint64_t>(slot) ^
                                      slot->size);
}

uint64_t NeededSize(uint64_t size) {
  uint64_t needed = size + sizeof(BuddySlot);
  // Start at 32 because sizeof(BuddySlot) is already 24;
  uint64_t pow2 = 32;

  while (pow2 < needed) {
    pow2 <<= 1;
  }

  return pow2;
}

class BuddyAllocator {
 public:
  BuddyAllocator() {}

  void Init() {
    free_front_ = nullptr;
    AddPage();
    check(ZMutexCreate(&mutex_cap_));
  }

  void* Allocate(uint64_t size) {
    check(ZMutexLock(mutex_cap_));
    if (size > (0x2000 - sizeof(BuddySlot))) {
      crash("Can't allocate greater than one page", glcr::UNIMPLEMENTED);
    }
    if (free_front_ == nullptr) {
      AddPage();
    }
    BuddySlot* slot = free_front_;
    uint64_t needed = NeededSize(size);
    BuddySlot* best_fit = nullptr;

    while (slot != nullptr) {
      bool fits = slot->size >= needed;
      bool better = best_fit == nullptr || slot->size < best_fit->size;
      if (fits && better) {
        best_fit = slot;
      }
      slot = slot->next;
    }

    if (best_fit == nullptr) {
      AddPage();
      best_fit = free_front_;
    }

    while (best_fit->size > needed) {
      best_fit = Split(best_fit);
    }

    Remove(best_fit);
    // TODO: We may need to align the pointer here.
    void* ptr = reinterpret_cast<uint8_t*>(best_fit) + sizeof(BuddySlot);
    check(ZMutexRelease(mutex_cap_));
    return ptr;
  }

  void Free(void* ptr) {
    check(ZMutexLock(mutex_cap_));
    BuddySlot* slot = ((BuddySlot*)ptr) - 1;
    if (slot->next || slot->prev) {
      crash("Double free", glcr::INTERNAL);
    }
    BuddySlot* buddy = GetBuddy(slot);
    while ((slot->size < 0x2000) && (buddy->next || buddy->prev) &&
           (buddy->size == slot->size)) {
      // Buddy is free! Merge!.
      Remove(buddy);
      if (buddy < slot) {
        slot = buddy;
      }
      slot->size *= 2;
      buddy = GetBuddy(slot);
    }

    if (free_front_) {
      free_front_->prev = slot;
    }
    slot->next = free_front_;
    free_front_ = slot;
    check(ZMutexRelease(mutex_cap_));
  }

 private:
  BuddySlot* free_front_ = nullptr;
  z_cap_t mutex_cap_ = 0;

  void AddPage() {
    uint64_t vaddr = PageAllocator::AllocatePagePair();
    BuddySlot* slot = reinterpret_cast<BuddySlot*>(vaddr);
    slot->prev = nullptr;
    slot->next = free_front_;
    if (free_front_) {
      free_front_->prev = slot;
    }
    free_front_ = slot;
    slot->size = 0x2000;
  }

  BuddySlot* Split(BuddySlot* slot) {
    if (slot->size <= 32) {
      crash("Splitting smallest buddy chunk", glcr::INTERNAL);
    }

    slot->size /= 2;
    BuddySlot* new_slot = GetBuddy(slot);
    new_slot->size = slot->size;
    new_slot->next = slot->next;
    new_slot->prev = slot;
    if (slot->next) {
      slot->next->prev = new_slot;
    }
    slot->next = new_slot;
    return slot;
  }

  void Remove(BuddySlot* slot) {
    if (slot->prev) {
      slot->prev->next = slot->next;
    }
    if (slot->next) {
      slot->next->prev = slot->prev;
    }

    if (free_front_ == slot) {
      free_front_ = slot->next;
    }

    slot->next = nullptr;
    slot->prev = nullptr;
  }
};

BuddyAllocator gAllocator;
bool gHasInit = false;

// FIXME: Race condition.
void* Allocate(uint64_t size) {
  if (!gHasInit) {
    // Call Init since we don't call global constructors yet.
    gAllocator.Init();
    gHasInit = true;
  }
  void* ptr = gAllocator.Allocate(size);

  char buffer[64];
  // glcr::FixedStringBuilder builder(buffer, 64);
  // glcr::StrFormatIntoBuffer(builder, "Allocated {x}", (uint64_t)ptr);
  // dbgln(builder.operator glcr::StringView());
  return ptr;
}

void Free(void* ptr) { gAllocator.Free(ptr); }

}  // namespace

[[nodiscard]] void* operator new(uint64_t size) { return Allocate(size); }
[[nodiscard]] void* operator new[](uint64_t size) { return Allocate(size); }

void operator delete(void* ptr, uint64_t) { Free(ptr); }
void operator delete[](void* ptr) { Free(ptr); }
void operator delete[](void* ptr, uint64_t) { Free(ptr); }
