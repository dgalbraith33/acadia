extern crate alloc;

use crate::zion;
use core::ffi::c_void;
use core::panic::PanicInfo;

#[must_use]
pub fn syscall<T>(id: u64, req: &T) -> Result<(), zion::ZError> {
    unsafe {
        let resp = zion::SysCall1(id, req as *const T as *const c_void);
        if resp != 0 {
            return Err(zion::ZError::from(resp));
        }
    }
    Ok(())
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    unsafe {
        if crate::mem::CAN_ALLOC {
            crate::debug!("Panic occured: {}", info);
        } else {
            debug("Panic occured before heap initialized.")
        }
    }
    // Internal error.
    let req = zion::ZProcessExitReq { code: 0x100 };
    let _ = syscall(zion::kZionProcessExit, &req);
    unreachable!()
}

pub fn debug(msg: &str) {
    let req = zion::ZDebugReq {
        message: msg.as_ptr() as *const i8,
        size: msg.len() as u64,
    };
    syscall(zion::kZionDebug, &req).expect("Failed to write");
}
