#![no_std]
#![no_main]

extern crate alloc;

use alloc::string::ToString;
use mammoth::debug;
use mammoth::define_entry;
use mammoth::thread;
use mammoth::zion::z_err_t;
use yellowstone_yunq::GetEndpointRequest;

define_entry!();

#[no_mangle]
pub extern "C" fn main() -> z_err_t {
    debug!("Testing!");

    let yellowstone = yellowstone_yunq::from_init_endpoint();

    debug!("Get endpoint");

    let endpoint = yellowstone
        .get_endpoint(&GetEndpointRequest {
            endpoint_name: "denali".to_string(),
        })
        .expect("Failed to get endpoint");

    debug!("Got endpoint w/ cap: {:#x}", endpoint.endpoint);

    let e: thread::ThreadEntry = |_| {
        debug!("Testing 1 2 3");
    };
    let t = thread::Thread::spawn(e, core::ptr::null()).expect("Failed to spawn thread");

    t.join().expect("Failed to wait.");

    0
}
