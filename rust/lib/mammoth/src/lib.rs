#![no_std]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use core::ffi::c_void;
use core::panic::PanicInfo;

include!("bindings.rs");

fn syscall<T>(id: u64, req: &T) -> u64 {
    unsafe { SysCall1(id, req as *const T as *const c_void) }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    let req = ZProcessExitReq { code: 1 };
    syscall(kZionProcessExit, &req);
    unreachable!()
}

pub fn debug(msg: &str) {
    let req = ZDebugReq {
        message: msg.as_ptr() as *const i8,
        size: msg.len() as u64,
    };
    syscall(kZionDebug, &req);
}
