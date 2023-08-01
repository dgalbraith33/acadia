#pragma once

#include <stdint.h>

struct Superblock {
  uint32_t inodes_count;
  uint32_t blocks_count;
  uint32_t reserved_blocks_count;
  uint32_t free_blocks_count;
  uint32_t free_inodes_count;
  uint32_t first_data_blok;
  uint32_t log_block_size;
  uint32_t log_frag_size;
  uint32_t blocks_per_group;
  uint32_t frags_per_group;
  uint32_t inodes_per_group;
  uint32_t mtime;
  uint32_t wtime;
  uint16_t mnt_count;
  uint16_t max_mnt_count;
  uint16_t magic;
  uint16_t state;
  uint16_t errors;
  uint16_t minor_rev_level;
  uint32_t lastcheck;
  uint32_t checkinterval;
  uint32_t creator_os;
  uint32_t rev_level;
  uint16_t def_resuid;
  uint16_t def_resgid;
  uint32_t first_ino;
  uint16_t inode_size;
} __attribute__((__packed__));

struct BlockGroupDescriptor {
  uint32_t block_bitmap;
  uint32_t inode_bitmap;
  uint32_t inode_table;
  uint16_t free_blocks_count;
  uint16_t free_inodes_count;
  uint16_t used_dirs_count;
  uint8_t reserved[14];
} __attribute__((__packed__));

struct Inode {
  uint16_t mode;
  uint16_t uid;
  uint32_t size;
  uint32_t atime;
  uint32_t ctime;
  uint32_t mtime;
  uint32_t dtime;
  uint16_t gid;
  uint16_t links_count;
  uint32_t blocks;
  uint32_t flags;
  uint32_t osd1;
  uint32_t block[15];
  uint32_t generation;
  uint32_t file_acl;
  uint32_t dir_acl;
  uint32_t faddr;
  uint32_t osd2[3];
} __attribute__((__packed__));

constexpr uint8_t kExt2FtUnknown = 0;
constexpr uint8_t kExt2FtFile = 1;
constexpr uint8_t kExt2FtDirectory = 2;

struct DirEntry {
  uint32_t inode;
  uint16_t record_length;
  uint8_t name_len;
  uint8_t file_type;
  char name[256];
} __attribute__((__packed__));
