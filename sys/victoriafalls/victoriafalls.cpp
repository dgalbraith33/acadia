#include <denali/scoped_denali_client.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "fs/ext2/ext2_driver.h"

uint64_t main(uint64_t init_cap) {
  ParseInitPort(init_cap);

  dbgln("VFs Started");

  YellowstoneClient yellowstone(gInitEndpointCap);
  Empty empty;
  DenaliInfo denali_info;
  RET_ERR(yellowstone.GetDenali(empty, denali_info));
  dbgln("LBA (recv): %u", denali_info.lba_offset());
  ScopedDenaliClient denali(denali_info.denali_endpoint(),
                            denali_info.device_id(), denali_info.lba_offset());
  ASSIGN_OR_RETURN(Ext2Driver ext2, Ext2Driver::Init(glcr::Move(denali)));

  ASSIGN_OR_RETURN(Inode * root, ext2.GetInode(2));
  check(ext2.ProbeDirectory(root));

  return 0;
}
