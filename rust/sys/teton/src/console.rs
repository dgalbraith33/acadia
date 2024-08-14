use crate::framebuffer::Framebuffer;
use crate::psf::Psf;

pub struct Console {
    framebuffer: Framebuffer,
    psf: Psf,
    row: u32,
    col: u32,
}

impl Console {
    pub fn new(framebuffer: Framebuffer, psf: Psf) -> Self {
        Self {
            framebuffer,
            psf,
            row: 0,
            col: 0,
        }
    }

    fn incr_cursor(&mut self) {
        self.col += 1;
        if self.col >= self.cols() {
            self.col = 0;
            self.row += 1;
        }

        if self.row >= self.rows() {
            panic!("Scroll unimplemented")
        }
    }

    pub fn write_char(&mut self, chr: char) {
        if chr == '\x08' {
            // Backspace.
            if self.col > 1 {
                self.col -= 1;
                self.write_char(' ');
                self.col -= 1;
            }
            return;
        }
        let glyph = self.psf.glyph(chr as u32);
        self.framebuffer.draw_glyph(
            glyph,
            self.row * (self.psf.height() + 1),
            self.col * (self.psf.width() + 1),
        );
        self.incr_cursor()
    }

    fn cols(&self) -> u32 {
        self.framebuffer.width() / (self.psf.width() + 1)
    }

    fn rows(&self) -> u32 {
        self.framebuffer.height() / (self.psf.height() + 1)
    }
}
