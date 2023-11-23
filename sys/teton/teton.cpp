#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>
#include <victoriafalls/victoriafalls.yunq.client.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "framebuffer/console.h"
#include "framebuffer/framebuffer.h"
#include "framebuffer/psf.h"

uint64_t main(uint64_t init_port) {
  ParseInitPort(init_port);

  dbgln("Teton Starting");

  // 1. Set up framebuffer.
  YellowstoneClient client(gInitEndpointCap);

  FramebufferInfo framebuffer;
  RET_ERR(client.GetFramebufferInfo({}, framebuffer));
  dbgln("FB addr {x}, bpp {}, width {} , height {}, pitch {}",
        framebuffer.address_phys(), framebuffer.bpp(), framebuffer.width(),
        framebuffer.height(), framebuffer.pitch());

  Framebuffer fbuf(framebuffer);

  // 2. Parse a font file.

  Psf psf("/default8x16.psfu");
  psf.DumpHeader();

  Console console(fbuf, psf);
  console.WriteString("Hello World!\n");
  for (uint8_t i = 0x20; i < 0x7E; i++) {
    console.WriteChar(i);
  }

  // 3. Write a line to the screen.

  return 0;
}
