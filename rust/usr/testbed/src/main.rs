#![no_std]
#![no_main]

extern crate alloc;

use alloc::boxed::Box;
use alloc::string::ToString;
use mammoth::debug;
use mammoth::define_entry;
use mammoth::syscall::debug;
use mammoth::thread;
use mammoth::zion::z_err_t;
use yellowstone::GetEndpointRequest;
use yellowstone::YellowstoneClient;

define_entry!();

#[no_mangle]
pub extern "C" fn main() -> z_err_t {
    debug("Testing!");
    let x = Box::new("Heap str");
    debug(&x);
    debug!("Formatted {}", "string");
    let mut vmmo_cap: u64 = 0;
    let obj_req = mammoth::zion::ZMemoryObjectCreateReq {
        size: 0,
        vmmo_cap: &mut vmmo_cap as *mut u64,
    };
    mammoth::syscall::syscall(mammoth::zion::kZionMemoryObjectCreate, &obj_req)
        .expect("Failed to create memory object");

    let mut yellowstone;
    unsafe {
        yellowstone = YellowstoneClient::new(mammoth::init::INIT_ENDPOINT);
    }

    let endpoint = yellowstone
        .get_endpoint(&GetEndpointRequest {
            endpoint_name: "denali".to_string(),
        })
        .expect("Failed to get endpoint");

    debug!("Got endpoint w/ cap: {:#x}", endpoint.endpoint);

    let e: thread::ThreadEntry = |_| {
        debug!("Testing 1 2 3");
    };
    let t = thread::Thread::spawn(&e, core::ptr::null());

    t.join().expect("Failed to wait.");

    0
}
