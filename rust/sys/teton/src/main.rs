#![no_std]
#![no_main]

extern crate alloc;

mod console;
mod framebuffer;
mod psf;

use mammoth::{debug, define_entry, zion::z_err_t};

define_entry!();

#[no_mangle]
extern "C" fn main() -> z_err_t {
    debug!("Teton Starting");

    let yellowstone = yellowstone::from_init_endpoint();
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
    console.write_char('>');

    /*

    Terminal terminal(console);
    terminal.Register();

    Thread lthread = terminal.Listen();

    check(lthread.Join());
    */

    0
}
