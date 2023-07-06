#include "fs/ext2/inode_table.h"

#include <mammoth/debug.h>

InodeTable::InodeTable(Ext2BlockReader& reader, BlockGroupDescriptor* bgdt)
    : ext2_reader_(reader),
      bgdt_(bgdt),
      num_block_groups_(((reader.GetSuperblock()->blocks_count - 1) /
                         reader.GetSuperblock()->blocks_per_group) +
                        1),
      inodes_per_group_(reader.GetSuperblock()->inodes_per_group) {
  inode_tables_.Resize(num_block_groups_);
  dbgln("EXT rev: %x", reader.GetSuperblock()->rev_level);
  inode_size_ = reader.GetSuperblock()->rev_level >= 1
                    ? reader.GetSuperblock()->inode_size
                    : 0x80;
}

glcr::ErrorOr<Inode*> InodeTable::GetInode(uint64_t inode_num) {
  uint64_t block_group_num = (inode_num - 1) / inodes_per_group_;
  ASSIGN_OR_RETURN(Inode * root, GetRootOfInodeTable(block_group_num));
  uint64_t local_index = (inode_num - 1) % inodes_per_group_;
  return reinterpret_cast<Inode*>(reinterpret_cast<uint64_t>(root) +
                                  local_index * inode_size_);
}

glcr::ErrorOr<Inode*> InodeTable::GetRootOfInodeTable(
    uint64_t block_group_num) {
  if (block_group_num > num_block_groups_) {
    return glcr::INVALID_ARGUMENT;
  }

  if (!inode_tables_[block_group_num]) {
    // FIXME: Don't hardcode block and inode size here.
    uint64_t inode_table_size =
        ((inodes_per_group_ * inode_size_ - 1) / 1024) + 1;
    dbgln("Reading %lx, %lx", bgdt_[block_group_num].inode_table,
          inode_table_size);
    ASSIGN_OR_RETURN(inode_tables_[block_group_num],
                     ext2_reader_.ReadBlocks(bgdt_[block_group_num].inode_table,
                                             inode_table_size));
  }
  return reinterpret_cast<Inode*>(inode_tables_[block_group_num].vaddr());
}
