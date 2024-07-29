extern crate alloc;

use crate::zion;
use crate::zion::z_cap_t;
use crate::zion::ZError;
use core::ffi::c_void;
use core::panic::PanicInfo;

#[must_use]
pub fn syscall<T>(id: u64, req: &T) -> Result<(), ZError> {
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

pub fn thread_create(proc_cap: z_cap_t) -> Result<z_cap_t, ZError> {
    let mut cap = 0;
    syscall(
        zion::kZionThreadCreate,
        &zion::ZThreadCreateReq {
            proc_cap,
            thread_cap: &mut cap as *mut z_cap_t,
        },
    )?;
    Ok(cap)
}

pub fn thread_start(thread_cap: z_cap_t, entry: u64, arg1: u64, arg2: u64) -> Result<(), ZError> {
    syscall(
        zion::kZionThreadStart,
        &zion::ZThreadStartReq {
            thread_cap,
            entry,
            arg1,
            arg2,
        },
    )
}

pub fn thread_wait(thread_cap: z_cap_t) -> Result<(), ZError> {
    syscall(zion::kZionThreadWait, &zion::ZThreadWaitReq { thread_cap })
}

pub fn thread_exit() -> ! {
    let _ = syscall(zion::kZionThreadExit, &zion::ZThreadExitReq {});
    unreachable!();
}
