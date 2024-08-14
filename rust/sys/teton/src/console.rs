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

    pub fn write_char(&mut self, chr: char, row: u32, col: u32) {
        if row >= self.rows() {
            panic!("Write at row {}, max is {}", row, self.rows())
        }
        if col >= self.cols() {
            panic!("Write at col {}, max is {}", col, self.cols())
        }
        let glyph = self.psf.glyph(chr as u32);
        self.framebuffer.draw_glyph(
            glyph,
            row * (self.psf.height() + 1),
            col * (self.psf.width() + 1),
        );
    }

    pub fn cols(&self) -> u32 {
        self.framebuffer.width() / (self.psf.width() + 1)
    }

    pub fn rows(&self) -> u32 {
        self.framebuffer.height() / (self.psf.height() + 1)
    }
}
