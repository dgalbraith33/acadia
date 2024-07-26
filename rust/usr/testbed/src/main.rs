#![no_std]
#![no_main]

extern crate alloc;

use alloc::boxed::Box;
use mammoth::debug;
use mammoth::define_entry;
use mammoth::syscall::debug;
use mammoth::syscall::z_err_t;
use mammoth::syscall::Writer;

define_entry!();

#[no_mangle]
pub extern "C" fn main() -> z_err_t {
    debug("Testing!");
    let x = Box::new("Heap str");
    debug(&x);
    debug!("Formatted {}", "string");
    let mut vmmo_cap: u64 = 0;
    let obj_req = mammoth::syscall::ZMemoryObjectCreateReq {
        size: 0,
        vmmo_cap: &mut vmmo_cap as *mut u64,
    };
    mammoth::syscall::syscall(mammoth::syscall::kZionMemoryObjectCreate, &obj_req)
        .expect("Failed to create memory object");
    0
}
