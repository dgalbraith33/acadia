#pragma once

#include <glacier/container/binary_tree.h>

#include "object/memory_object.h"

/* AddressRangeTree stores memory objects referred to by
 * ranges and ensures those ranges do not overlap.
 */
class MemoryMappingTree {
 public:
  MemoryMappingTree() = default;

  MemoryMappingTree(const MemoryMappingTree&) = delete;
  MemoryMappingTree(MemoryMappingTree&&) = delete;

  glcr::ErrorCode AddInMemoryObject(uint64_t vaddr,
                                    const glcr::RefPtr<MemoryObject>& object);

  glcr::ErrorCode FreeMemoryRange(uint64_t vaddr_base, uint64_t vaddr_limit);

  glcr::ErrorOr<uint64_t> GetPhysicalPageAtVaddr(uint64_t vaddr);

 private:
  struct MemoryMapping {
    uint64_t vaddr_base;
    uint64_t vaddr_limit;
    glcr::RefPtr<MemoryObject> mem_object;
  };

  // TODO: Consider adding a red-black tree implementation here.
  // As is this tree functions about as well as a linked list
  // because mappings are likely to be added in near-perfect ascedning order.
  // Also worth considering creating a special tree implementation for
  // just this purpose, or maybe a BinaryTree implementation that accepts
  // ranges rather than a single key.
  glcr::BinaryTree<uint64_t, MemoryMapping> mapping_tree_;

  glcr::Optional<glcr::Ref<MemoryMapping>> GetMemoryMappingForAddr(
      uint64_t vaddr);
};
