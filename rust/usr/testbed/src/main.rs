#![no_std]
#![no_main]

use mammoth::debug;

#[no_mangle]
pub extern "C" fn _start() -> ! {
    debug("Test!");
    panic!()
}
