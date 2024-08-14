use mammoth::{mem::MemoryRegion, zion::ZError};
use yellowstone::FramebufferInfo;

pub struct Framebuffer {
    fb_info: FramebufferInfo,
    memory_region: MemoryRegion,
}

impl Framebuffer {
    pub fn from_info(fb_info: FramebufferInfo) -> Result<Self, ZError> {
        let size = fb_info.height * fb_info.pitch;
        let memory_region = MemoryRegion::direct_physical(fb_info.address_phys, size)?;
        Ok(Self {
            fb_info,
            memory_region,
        })
    }

    fn draw_pixel(&self, row: u32, col: u32, pixel: u32) {
        let index = row * (self.fb_info.pitch as u32 / 4) + col;
        self.memory_region.mut_slice()[index as usize] = pixel;
    }

    pub fn draw_glyph(&self, glyph: &[u8], row: u32, col: u32) {
        let gl_width = 8;
        let gl_height = 16;

        for r in 0..gl_height {
            for c in 0..gl_width {
                if ((glyph[r] >> c) % 2) == 1 {
                    self.draw_pixel(row + (r as u32), col + (gl_width - c - 1), 0xFFFFFFFF);
                } else {
                    self.draw_pixel(row + (r as u32), col + (gl_width - c - 1), 0);
                }
            }
        }
    }

    pub fn width(&self) -> u32 {
        self.fb_info.width as u32
    }

    pub fn height(&self) -> u32 {
        self.fb_info.height as u32
    }
}
