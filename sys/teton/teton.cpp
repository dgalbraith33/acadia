#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <yellowstone/yellowstone.yunq.client.h>

uint64_t main(uint64_t init_port) {
  ParseInitPort(init_port);

  dbgln("Teton Starting");

  // 1. Set up framebuffer.
  YellowstoneClient client(gInitEndpointCap);

  FramebufferInfo framebuffer;
  RET_ERR(client.GetFramebufferInfo({}, framebuffer));
  dbgln("FB addr {x}, width {} , height {}", framebuffer.address_phys(),
        framebuffer.width(), framebuffer.height());

  // 2. Parse a font file.

  // 3. Write a line to the screen.

  return 0;
}
