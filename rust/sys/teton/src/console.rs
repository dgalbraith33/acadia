use crate::framebuffer::Framebuffer;
use crate::psf::Psf;

pub struct Console {
    framebuffer: Framebuffer,
    psf: Psf,
}

impl Console {
    pub fn new(framebuffer: Framebuffer, psf: Psf) -> Self {
        Self { framebuffer, psf }
    }

    pub fn write_char(&self, c: char) {
        let glyph = self.psf.glyph(c as u32);
        self.framebuffer.draw_glyph(glyph, 0, 0)
    }
}
