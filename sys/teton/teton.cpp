#include <mammoth/debug.h>
#include <mammoth/init.h>

uint64_t main(uint64_t init_port) {
  ParseInitPort(init_port);

  dbgln("Teton Starting");
  // 1. Set up framebuffer.

  // 2. Parse a font file.

  // 3. Write a line to the screen.

  return 0;
}
