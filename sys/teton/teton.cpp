#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <victoriafalls/victoriafalls.yunq.client.h>
#include <yellowstone/yellowstone.yunq.client.h>

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

  for (uint64_t r = 0; r < 20; r++) {
    for (uint64_t c = 0; c < 20; c++) {
      fbuf.DrawPixel(r, c, 0x0000FF00);
    }
  }

  // 2. Parse a font file.

  GetEndpointRequest req;
  req.set_endpoint_name("victoriafalls");
  Endpoint resp;
  check(client.GetEndpoint(req, resp));

  VFSClient vfs(resp.endpoint());

  OpenFileRequest freq;
  freq.set_path("/default8x16.psfu");
  OpenFileResponse fresp;
  check(vfs.OpenFile(freq, fresp));

  Psf psf(OwnedMemoryRegion::FromCapability(fresp.memory()));
  psf.DumpHeader();

  fbuf.DrawGlyph(psf.glyph('C'));

  // 3. Write a line to the screen.

  return 0;
}
