#include "lib/memory_mapping_tree.h"

#include "debug/debug.h"

glcr::ErrorCode MemoryMappingTree::AddInMemoryObject(
    uint64_t vaddr, const glcr::RefPtr<MemoryObject>& object) {
  // TODO: This implementation is inefficient as it traverses the tree a lot, we
  // should have some solution with iterators to avoid this.
  auto predecessor_or = mapping_tree_.Predecessor(vaddr);
  if (predecessor_or && predecessor_or.value().get().vaddr_limit > vaddr) {
    return glcr::ALREADY_EXISTS;
  }
  if (mapping_tree_.Find(vaddr)) {
    return glcr::ALREADY_EXISTS;
  }
  auto successor_or = mapping_tree_.Successor(vaddr);
  if (successor_or &&
      successor_or.value().get().vaddr_base < vaddr + object->size()) {
    return glcr::ALREADY_EXISTS;
  }

  mapping_tree_.Insert(vaddr, MemoryMapping{
                                  .vaddr_base = vaddr,
                                  .vaddr_limit = vaddr + object->size(),
                                  .mem_object = object,
                              });

  return glcr::OK;
}

glcr::ErrorCode MemoryMappingTree::FreeMemoryRange(uint64_t vaddr_base,
                                                   uint64_t vaddr_limit) {
  if (vaddr_limit <= vaddr_base) {
    return glcr::INVALID_ARGUMENT;
  }
  auto predecessor_or = mapping_tree_.Predecessor(vaddr_base);
  if (predecessor_or && predecessor_or.value().get().vaddr_limit > vaddr_base) {
    return glcr::FAILED_PRECONDITION;
  }
  auto last_predecessor_or = mapping_tree_.Predecessor(vaddr_limit);
  if (last_predecessor_or &&
      last_predecessor_or.value().get().vaddr_limit > vaddr_limit) {
    return glcr::FAILED_PRECONDITION;
  }

  auto find_or = mapping_tree_.Find(vaddr_base);
  if (find_or) {
    mapping_tree_.Delete(vaddr_base);
  }
  while (true) {
    auto successor_or = mapping_tree_.Successor(vaddr_base);
    if (!successor_or || successor_or.value().get().vaddr_base >= vaddr_limit) {
      return glcr::OK;
    }
    mapping_tree_.Delete(successor_or.value().get().vaddr_base);
  }
}

glcr::ErrorOr<uint64_t> MemoryMappingTree::GetPhysicalPageAtVaddr(
    uint64_t vaddr) {
  auto mapping_or = GetMemoryMappingForAddr(vaddr);
  if (!mapping_or) {
    return glcr::NOT_FOUND;
  }
  MemoryMapping& mapping = mapping_or.value();
  return mapping.mem_object->PhysicalPageAtOffset(vaddr - mapping.vaddr_base);
}

glcr::Optional<glcr::Ref<MemoryMappingTree::MemoryMapping>>
MemoryMappingTree::GetMemoryMappingForAddr(uint64_t vaddr) {
  auto mapping_or = mapping_tree_.Predecessor(vaddr + 1);
  if (!mapping_or) {
    return mapping_or;
  }
  MemoryMapping& mapping = mapping_or.value();
  if (mapping.vaddr_base + mapping.mem_object->size() <= vaddr) {
    return {};
  }
  return mapping_or;
}
