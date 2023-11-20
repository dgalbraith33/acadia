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

glcr::ErrorCode FreeMemoryRange(uint64_t vaddr_base, uint64_t vaddr_limit) {
  dbgln("Unhandled free memory range!");
  return glcr::OK;
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
