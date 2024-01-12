#include "fs/ext2/ext2_block_reader.h"

#include <mammoth/util/debug.h>

glcr::ErrorOr<glcr::SharedPtr<Ext2BlockReader>> Ext2BlockReader::Init(
    const yellowstone::DenaliInfo& denali_info) {
  // Read 1024 bytes from 1024 offset.
  // FIXME: Don't assume 512 byte sectors somehow.
  DenaliClient client(denali_info.denali_endpoint());
  ReadRequest req;
  req.set_device_id(denali_info.device_id());
  req.mutable_block().set_lba(denali_info.lba_offset() + 2);
  req.mutable_block().set_size(2);
  ReadResponse resp;
  auto status = client.Read(req, resp);
  if (!status.ok()) {
    dbgln("Failed to read superblock: {}", status.message());
    return status.code();
  }
  mmth::OwnedMemoryRegion superblock =
      mmth::OwnedMemoryRegion::FromCapability(resp.memory());

  return glcr::SharedPtr<Ext2BlockReader>(
      new Ext2BlockReader(glcr::Move(client), denali_info.device_id(),
                          denali_info.lba_offset(), glcr::Move(superblock)));
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

glcr::ErrorOr<mmth::OwnedMemoryRegion> Ext2BlockReader::ReadBlock(
    uint64_t block_number) {
  return ReadBlocks(block_number, 1);
}
glcr::ErrorOr<mmth::OwnedMemoryRegion> Ext2BlockReader::ReadBlocks(
    uint64_t block_number, uint64_t num_blocks) {
  ReadRequest req;
  req.set_device_id(device_id_);
  req.mutable_block().set_lba(lba_offset_ + block_number * SectorsPerBlock());
  req.mutable_block().set_size(num_blocks * SectorsPerBlock());
  ReadResponse resp;
  auto status = denali_.Read(req, resp);
  if (!status.ok()) {
    dbgln("Failed to read blocks: {}", status.message());
    return status.code();
  }
  return mmth::OwnedMemoryRegion::FromCapability(resp.memory());
}

glcr::ErrorOr<mmth::OwnedMemoryRegion> Ext2BlockReader::ReadBlocks(
    const glcr::Vector<uint64_t>& block_list) {
  ReadManyRequest req;
  req.set_device_id(device_id_);
  for (uint64_t i = 0; i < block_list.size(); i++) {
    uint64_t curr_start = lba_offset_ + block_list.at(i) * SectorsPerBlock();
    uint64_t curr_run_len = 1;
    while ((i + 1) < block_list.size() &&
           block_list.at(i + 1) == block_list.at(i) + 1) {
      i++;
      curr_run_len++;
    }
    req.add_lba(curr_start);
    req.add_sector_cnt(curr_run_len * SectorsPerBlock());
  }
  ReadResponse resp;
  auto status = denali_.ReadMany(req, resp);
  if (!status.ok()) {
    dbgln("Failed to read blocks: {}", status.message());
    return status.code();
  }
  return mmth::OwnedMemoryRegion::FromCapability(resp.memory());
}

Ext2BlockReader::Ext2BlockReader(DenaliClient&& denali, uint64_t device_id,
                                 uint64_t lba_offset,
                                 mmth::OwnedMemoryRegion&& super_block)
    : denali_(glcr::Move(denali)),
      device_id_(device_id),
      lba_offset_(lba_offset),
      super_block_region_(glcr::Move(super_block)) {}
