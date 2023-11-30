#include "fs/ext2/ext2_driver.h"

#include <glacier/string/string.h>
#include <mammoth/util/debug.h>

glcr::ErrorOr<Ext2Driver> Ext2Driver::Init(
    const yellowstone::DenaliInfo& denali_info) {
  ASSIGN_OR_RETURN(glcr::SharedPtr<Ext2BlockReader> reader,
                   Ext2BlockReader::Init(glcr::Move(denali_info)));

  ASSIGN_OR_RETURN(
      mmth::OwnedMemoryRegion bgdt,
      reader->ReadBlocks(reader->BgdtBlockNum(), reader->BgdtBlockSize()));
  glcr::UniquePtr<InodeTable> inode_table(
      new InodeTable(reader, glcr::Move(bgdt)));

  return Ext2Driver(reader, glcr::Move(inode_table));
}

glcr::ErrorCode Ext2Driver::ProbePartition() {
  Superblock* superblock = ext2_reader_->GetSuperblock();
  if (superblock->magic != 0xEF53) {
    dbgln("Invalid EXT2 magic code: {x}");
    return glcr::INVALID_ARGUMENT;
  }
  dbgln("Block size: 0x{x}", 1024 << superblock->log_block_size);

  dbgln("Blocks: 0x{x} (0x{x} per group)", superblock->blocks_count,
        superblock->blocks_per_group);
  dbgln("Inodes: 0x{x} (0x{x} per group)", superblock->inodes_count,
        superblock->inodes_per_group);
  dbgln("Inode size: 0x{x}", superblock->inode_size);

  dbgln("Mounts: 0x{x} out of 0x{x}", superblock->mnt_count,
        superblock->max_mnt_count);
  dbgln("State: {x}", superblock->state);

  dbgln("Created by: {x}", superblock->creator_os);

  return glcr::OK;
}

glcr::ErrorOr<Inode*> Ext2Driver::GetInode(uint32_t inode_number) {
  return inode_table_->GetInode(inode_number);
}

glcr::ErrorOr<glcr::Vector<DirEntry>> Ext2Driver::ReadDirectory(
    uint32_t inode_number) {
  ASSIGN_OR_RETURN(Inode * inode, inode_table_->GetInode(inode_number));
  if (!(inode->mode & 0x4000)) {
    dbgln("Reading non directory.");
    return glcr::INVALID_ARGUMENT;
  }

  // This calculation is cursed.
  uint64_t real_block_cnt =
      (inode->blocks - 1) / (ext2_reader_->BlockSize() / 512) + 1;

  if (real_block_cnt > 12) {
    dbgln("Cant handle indirect blocks yet");
    return glcr::FAILED_PRECONDITION;
  }

  glcr::Vector<DirEntry> directory;
  for (uint64_t i = 0; i < real_block_cnt; i++) {
    dbgln("Getting block {x}", inode->block[i]);
    ASSIGN_OR_RETURN(mmth::OwnedMemoryRegion block,
                     ext2_reader_->ReadBlock(inode->block[i]));
    uint64_t addr = block.vaddr();
    while (addr < block.vaddr() + ext2_reader_->BlockSize()) {
      DirEntry* entry = reinterpret_cast<DirEntry*>(addr);
      directory.PushBack(*entry);
      glcr::StringView name(entry->name, entry->name_len);
      switch (entry->file_type) {
        case kExt2FtFile:
          dbgln("FILE (0x{x}): {}", entry->inode, name);
          break;
        case kExt2FtDirectory:
          dbgln("DIR  (0x{x}): {}", entry->inode, name);
          break;
        default:
          dbgln("UNK  (0x{x}): {}", entry->inode, name);
      }
      addr += entry->record_length;
    }
  }
  return directory;
}

glcr::ErrorOr<mmth::OwnedMemoryRegion> Ext2Driver::ReadFile(
    uint64_t inode_number) {
  ASSIGN_OR_RETURN(Inode * inode, inode_table_->GetInode(inode_number));

  if (!(inode->mode & 0x8000)) {
    dbgln("Reading non file.");
    return glcr::INVALID_ARGUMENT;
  }

  // This calculation is cursed.
  uint64_t real_block_cnt =
      (inode->blocks - 1) / (ext2_reader_->BlockSize() / 512) + 1;

  if (inode->block[14]) {
    dbgln("Can't handle triply-indirect blocks yet.");
    return glcr::UNIMPLEMENTED;
  }

  mmth::OwnedMemoryRegion double_indirect_block;
  if (inode->block[13]) {
    ASSIGN_OR_RETURN(double_indirect_block,
                     ext2_reader_->ReadBlock(inode->block[13]));
  }

  mmth::OwnedMemoryRegion indirect_block;
  if (inode->block[12]) {
    ASSIGN_OR_RETURN(indirect_block, ext2_reader_->ReadBlock(inode->block[12]));
  }

  glcr::Vector<uint64_t> blocks_to_read;
  for (uint64_t i = 0; i < 12 && i < real_block_cnt; i++) {
    blocks_to_read.PushBack(inode->block[i]);
  }

  uint32_t* indr_block_array =
      reinterpret_cast<uint32_t*>(indirect_block.vaddr());
  for (uint64_t i = 12; i < 268 && i < real_block_cnt; i++) {
    uint64_t offset = i - 12;
    blocks_to_read.PushBack(indr_block_array[offset]);
  }

  uint32_t* dbl_indr_block_array =
      reinterpret_cast<uint32_t*>(double_indirect_block.vaddr());
  for (uint64_t i = 0; i < 256; i++) {
    uint64_t block = 268 + (256 * i);
    if (block >= real_block_cnt) {
      break;
    }
    ASSIGN_OR_RETURN(mmth::OwnedMemoryRegion single_indr_block,
                     ext2_reader_->ReadBlock(dbl_indr_block_array[i]));
    uint32_t* single_indr_block_array =
        reinterpret_cast<uint32_t*>(single_indr_block.vaddr());
    for (uint64_t j = 0; j < 256; j++) {
      uint64_t block_inner = block + j;
      if (block_inner >= real_block_cnt) {
        break;
      }
      blocks_to_read.PushBack(single_indr_block_array[j]);
    }
  }

  return ext2_reader_->ReadBlocks(blocks_to_read);
}
