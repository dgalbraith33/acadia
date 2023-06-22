#include <mammoth/debug.h>
#include <mammoth/init.h>

uint64_t main(uint64_t init_cap) {
  ParseInitPort(init_cap);

  dbgln("VFs Started");
  return 0;
}
