#include <mammoth/util/debug.h>
#include <zcall.h>

uint64_t main(uint64_t init_port_cap) {
  dbgln("testbed");
  check(ZThreadSleep(2000));
  dbgln("testbed2");

  return glcr::OK;
}
