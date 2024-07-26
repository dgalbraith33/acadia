use core::ffi::c_void;
use core::panic::PanicInfo;

include!("bindings.rs");

pub fn syscall<T>(id: u64, req: &T) -> u64 {
    unsafe { SysCall1(id, req as *const T as *const c_void) }
}

pub fn checked_syscall<T>(id: u64, req: &T) {
    if syscall(id, req) != 0 {
        debug("Bad syscall response.");
        panic!();
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    // Internal error.
    let req = ZProcessExitReq { code: 0x100 };
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
