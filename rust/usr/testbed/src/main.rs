#![no_std]
#![no_main]

use mammoth::debug;
use mammoth::define_entry;
use mammoth::z_err_t;

define_entry!();

#[no_mangle]
pub extern "C" fn main() -> z_err_t {
    debug("Testing!");
    0
}
