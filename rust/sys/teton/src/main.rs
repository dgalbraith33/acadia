#![no_std]
#![no_main]

extern crate alloc;

mod console;
mod framebuffer;
mod psf;
mod terminal;

use core::cell::RefCell;

use alloc::rc::Rc;
use mammoth::{debug, define_entry, zion::z_err_t};
use voyageurs::listener::KeyboardListener;

define_entry!();

#[no_mangle]
extern "C" fn main() -> z_err_t {
    debug!("Teton Starting");

    let yellowstone = yellowstone_yunq::from_init_endpoint();
    let framebuffer_info = yellowstone
        .get_framebuffer_info()
        .expect("Failed to get framebuffer info.");

    debug!(
        "FB addr {:#x}, bpp {}, width {} , height {}, pitch {}",
        framebuffer_info.address_phys,
        framebuffer_info.bpp,
        framebuffer_info.width,
        framebuffer_info.height,
        framebuffer_info.pitch
    );

    let framebuffer = framebuffer::Framebuffer::from_info(framebuffer_info)
        .expect("Failed to create framebuffer");

    let psf = psf::Psf::new("/default8x16.psfu").expect("Failed to open font file.");
    let console = console::Console::new(framebuffer, psf);
    let terminal = Rc::new(RefCell::new(terminal::Terminal::new(console)));

    let kb_listener = KeyboardListener::new(terminal).expect("Failed to create keyboard listener");

    kb_listener
        .join()
        .expect("Failed to wait on keyboard listener");

    0
}
