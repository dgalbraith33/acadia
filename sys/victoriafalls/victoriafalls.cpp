#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <yellowstone_stub.h>

#include "fs/ext2/ext2_driver.h"

uint64_t main(uint64_t init_cap) {
  ParseInitPort(init_cap);

  dbgln("VFs Started");

  YellowstoneStub yellowstone(gInitEndpointCap);
  ASSIGN_OR_RETURN(ScopedDenaliClient denali, yellowstone.GetDenali());
  ASSIGN_OR_RETURN(Ext2Driver ext2, Ext2Driver::Init(glcr::Move(denali)));

  ASSIGN_OR_RETURN(Inode * root, ext2.GetInode(2));
  check(ext2.ProbeDirectory(root));

  return 0;
}
