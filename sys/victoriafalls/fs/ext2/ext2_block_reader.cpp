#include "fs/ext2/ext2_block_reader.h"

#include "mammoth/debug.h"

glcr::ErrorOr<glcr::SharedPtr<Ext2BlockReader>> Ext2BlockReader::Init(
    ScopedDenaliClient&& denali) {
  // Read 1024 bytes from 1024 offset.
  // FIXME: Don't assume 512 byte sectors somehow.
  ASSIGN_OR_RETURN(MappedMemoryRegion superblock, denali.ReadSectors(2, 2));
  return glcr::SharedPtr<Ext2BlockReader>(
      new Ext2BlockReader(glcr::Move(denali), superblock));
}

Superblock* Ext2BlockReader::GetSuperblock() {
  return reinterpret_cast<Superblock*>(super_block_region_.vaddr());
}

uint64_t Ext2BlockReader::SectorsPerBlock() {
  return 1 << (GetSuperblock()->log_block_size + 1);
}

uint64_t Ext2BlockReader::BlockSize() {
  return 1024 << (GetSuperblock()->log_block_size);
}

uint64_t Ext2BlockReader::NumberOfBlockGroups() {
  return ((GetSuperblock()->blocks_count - 1) /
          GetSuperblock()->blocks_per_group) +
         1;
}

uint64_t Ext2BlockReader::BgdtBlockNum() {
  return (BlockSize() == 1024) ? 2 : 1;
}

uint64_t Ext2BlockReader::BgdtBlockSize() {
  return ((NumberOfBlockGroups() * sizeof(BlockGroupDescriptor) - 1) /
          BlockSize()) +
         1;
}

uint64_t Ext2BlockReader::InodeSize() {
  constexpr uint64_t kDefaultInodeSize = 0x80;
  return GetSuperblock()->rev_level >= 1 ? GetSuperblock()->inode_size
                                         : kDefaultInodeSize;
}

uint64_t Ext2BlockReader::InodeTableBlockSize() {
  return (InodeSize() * GetSuperblock()->inodes_per_group) / BlockSize();
}

glcr::ErrorOr<MappedMemoryRegion> Ext2BlockReader::ReadBlock(
    uint64_t block_number) {
  return denali_.ReadSectors(block_number * SectorsPerBlock(),
                             SectorsPerBlock());
}
glcr::ErrorOr<MappedMemoryRegion> Ext2BlockReader::ReadBlocks(
    uint64_t block_number, uint64_t num_blocks) {
  return denali_.ReadSectors(block_number * SectorsPerBlock(),
                             num_blocks * SectorsPerBlock());
}

Ext2BlockReader::Ext2BlockReader(ScopedDenaliClient&& denali,
                                 MappedMemoryRegion super_block)
    : denali_(glcr::Move(denali)), super_block_region_(super_block) {}
