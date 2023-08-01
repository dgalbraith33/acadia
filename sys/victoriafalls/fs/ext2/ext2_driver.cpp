#include "fs/ext2/ext2_driver.h"

#include <glacier/string/string.h>
#include <mammoth/debug.h>

glcr::ErrorOr<Ext2Driver> Ext2Driver::Init(ScopedDenaliClient&& denali) {
  ASSIGN_OR_RETURN(Ext2BlockReader reader,
                   Ext2BlockReader::Init(glcr::Move(denali)));
  Superblock* superblock = reader.GetSuperblock();
  return Ext2Driver(glcr::Move(reader));
}

glcr::ErrorCode Ext2Driver::ProbePartition() {
  Superblock* superblock = ext2_reader_.GetSuperblock();
  if (superblock->magic != 0xEF53) {
    dbgln("Invalid EXT2 magic code: %x");
    return glcr::INVALID_ARGUMENT;
  }
  dbgln("Block size: 0x%x", 1024 << superblock->log_block_size);

  dbgln("Blocks: 0x%x (0x%x per group)", superblock->blocks_count,
        superblock->blocks_per_group);
  dbgln("Inodes: 0x%x (0x%x per group)", superblock->inodes_count,
        superblock->inodes_per_group);
  dbgln("Inode size: 0x%x", superblock->inode_size);

  dbgln("Mounts: 0x%x out of 0x%x", superblock->mnt_count,
        superblock->max_mnt_count);
  dbgln("State: %x", superblock->state);

  dbgln("Created by: %x", superblock->creator_os);

  ASSIGN_OR_RETURN(MappedMemoryRegion bgdt,
                   ext2_reader_.ReadBlocks(ext2_reader_.BgdtBlockNum(),
                                           ext2_reader_.BgdtBlockSize()));
  BlockGroupDescriptor* bgds =
      reinterpret_cast<BlockGroupDescriptor*>(bgdt.vaddr());

  uint64_t num_block_groups = ext2_reader_.NumberOfBlockGroups();
  for (uint64_t i = 0; i < num_block_groups; i++) {
    dbgln("BGD %x", i);
    dbgln("Block Bitmap: %x", bgds[i].block_bitmap);
    dbgln("Inode Bitmap: %x", bgds[i].inode_bitmap);
    dbgln("Inode Table: %x", bgds[i].inode_table);
    dbgln("Free blocks: %x", bgds[i].free_blocks_count);
    dbgln("Free inodes: %x", bgds[i].free_inodes_count);
  }

  // FIXME: Move this to initialization.
  inode_table_ =
      glcr::UniquePtr<InodeTable>(new InodeTable(ext2_reader_, bgds));

  ASSIGN_OR_RETURN(Inode * root, inode_table_->GetInode(2));
  dbgln("Inode %lx", root);
  dbgln("Mode: %x", root->mode);
  dbgln("Blocks: %x", root->blocks);
  dbgln("Size: %x", root->size);

  return ProbeDirectory(root);
}

glcr::ErrorCode Ext2Driver::ProbeDirectory(Inode* inode) {
  if (!(inode->mode & 0x4000)) {
    dbgln("Probing non-directory");
    return glcr::INVALID_ARGUMENT;
  }

  // This calculation is cursed.
  uint64_t real_block_cnt =
      (inode->blocks - 1) / (ext2_reader_.BlockSize() / 512) + 1;

  if (real_block_cnt > 12) {
    dbgln("Cant handle indirect blocks yet");
    return glcr::FAILED_PRECONDITION;
  }

  for (uint64_t i = 0; i < real_block_cnt; i++) {
    dbgln("Getting block %lx", inode->block[i]);
    ASSIGN_OR_RETURN(MappedMemoryRegion block,
                     ext2_reader_.ReadBlock(inode->block[i]));
    uint64_t addr = block.vaddr();
    while (addr < block.vaddr() + ext2_reader_.BlockSize()) {
      DirEntry* entry = reinterpret_cast<DirEntry*>(addr);
      dbgln("Entry: inode: %x, rec_len %x, name_len %x, file_type: %x",
            entry->inode, entry->record_length, entry->name_len,
            entry->file_type);
      // FIXME: We need a method to construct a string that is *up to* a certain
      // length. As illustrated below the provided name_len is 0x2e but the
      // first character is \0 since this is the self-referencial inode.
      glcr::String name(entry->name, entry->name_len);
      dbgln("%s", name.cstr());
      addr += entry->record_length;
    }
  }
  return glcr::OK;
}
