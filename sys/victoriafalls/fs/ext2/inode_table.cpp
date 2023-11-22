#include "fs/ext2/inode_table.h"

InodeTable::InodeTable(const glcr::SharedPtr<Ext2BlockReader>& reader,
                       mmth::OwnedMemoryRegion&& bgdt_region)
    : ext2_reader_(reader),
      bgdt_region_(glcr::Move(bgdt_region)),
      bgdt_(reinterpret_cast<BlockGroupDescriptor*>(bgdt_region_.vaddr())) {
  inode_tables_.Resize(ext2_reader_->NumberOfBlockGroups());
}

glcr::ErrorOr<Inode*> InodeTable::GetInode(uint32_t inode_num) {
  uint64_t inodes_per_group = ext2_reader_->GetSuperblock()->inodes_per_group;
  uint64_t block_group_num = (inode_num - 1) / inodes_per_group;
  ASSIGN_OR_RETURN(Inode * root, GetRootOfInodeTable(block_group_num));
  uint64_t local_index = (inode_num - 1) % inodes_per_group;
  return reinterpret_cast<Inode*>(reinterpret_cast<uint64_t>(root) +
                                  local_index * ext2_reader_->InodeSize());
}

glcr::ErrorOr<Inode*> InodeTable::GetRootOfInodeTable(
    uint64_t block_group_num) {
  if (block_group_num > ext2_reader_->NumberOfBlockGroups()) {
    return glcr::INVALID_ARGUMENT;
  }

  if (!inode_tables_[block_group_num]) {
    ASSIGN_OR_RETURN(
        inode_tables_[block_group_num],
        ext2_reader_->ReadBlocks(bgdt_[block_group_num].inode_table,
                                 ext2_reader_->InodeTableBlockSize()));
  }
  return reinterpret_cast<Inode*>(inode_tables_[block_group_num].vaddr());
}
