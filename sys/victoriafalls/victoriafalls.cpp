#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <yellowstone_stub.h>

struct Superblock {
  uint32_t inode_count;
  uint32_t blocks_count;
  uint32_t reserved_blocks_count;
  uint32_t free_blocks_count;
  uint32_t free_inodes_count;
};

uint64_t main(uint64_t init_cap) {
  ParseInitPort(init_cap);

  dbgln("VFs Started");

  YellowstoneStub yellowstone(gInitEndpointCap);
  ASSIGN_OR_RETURN(ScopedDenaliClient denali, yellowstone.GetDenali());

  ASSIGN_OR_RETURN(MappedMemoryRegion region, denali.ReadSectors(2, 2));
  Superblock* super = reinterpret_cast<Superblock*>(region.vaddr());

  dbgln("Inodes: %x / %x", super->free_inodes_count, super->inode_count);
  dbgln("Blocks: %x / %x", super->free_blocks_count, super->blocks_count);

  return 0;
}
