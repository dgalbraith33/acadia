#![no_std]
#![no_main]

extern crate alloc;

use alloc::boxed::Box;
use mammoth::debug;
use mammoth::define_entry;
use mammoth::syscall::debug;
use mammoth::syscall::z_err_t;

define_entry!();

#[no_mangle]
pub extern "C" fn main() -> z_err_t {
    debug("Testing!");
    let x = Box::new("Heap str");
    debug(&x);
    debug!("Formatted {}", "string");
    0
}
