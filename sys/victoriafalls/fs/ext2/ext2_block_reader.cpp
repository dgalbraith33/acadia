#include "fs/ext2/ext2_block_reader.h"

glcr::ErrorOr<Ext2BlockReader> Ext2BlockReader::Init(
    ScopedDenaliClient&& denali) {
  // Read 1024 bytes from 1024 offset.
  // FIXME: Don't assume 512 byte sectors somehow.
  ASSIGN_OR_RETURN(MappedMemoryRegion superblock, denali.ReadSectors(2, 2));
  return Ext2BlockReader(glcr::Move(denali), superblock);
}

Superblock* Ext2BlockReader::GetSuperblock() {
  return reinterpret_cast<Superblock*>(super_block_region_.vaddr());
}

uint64_t Ext2BlockReader::SectorsPerBlock() {
  return 1 << (GetSuperblock()->log_block_size + 1);
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
