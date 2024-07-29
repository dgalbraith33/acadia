use crate::syscall;
use crate::syscall::z_cap_t;

use core::ffi::c_void;

pub type ThreadEntry = fn(*const c_void) -> ();

#[no_mangle]
extern "C" fn entry_point(entry_ptr: *const ThreadEntry, arg1: *const c_void) -> ! {
    debug!("Entry {:#p} arg1 {:#x}", entry_ptr, arg1 as u64);
    let entry = unsafe { *entry_ptr };

    entry(arg1);

    let _ = syscall::syscall(syscall::kZionThreadExit, &syscall::ZThreadExitReq {});

    unreachable!();
}

// TODO: Add a Drop implementation that kills this thread and drops its capability.
pub struct Thread<'a> {
    cap: z_cap_t,
    // This field only exists to ensure that the entry reference will outlive the thread object
    // itself.
    _entry: &'a ThreadEntry,
}

impl<'a> Thread<'a> {
    pub fn spawn(entry: &'a ThreadEntry, arg1: *const c_void) -> Self {
        let mut cap: z_cap_t = 0;
        let req = syscall::ZThreadCreateReq {
            proc_cap: unsafe { crate::SELF_PROC_CAP },
            thread_cap: &mut cap as *mut z_cap_t,
        };

        syscall::syscall(syscall::kZionThreadCreate, &req).expect("Failed to create thread.");

        syscall::syscall(
            syscall::kZionThreadStart,
            &syscall::ZThreadStartReq {
                thread_cap: cap,
                entry: entry_point as u64,
                arg1: entry as *const ThreadEntry as u64,
                arg2: arg1 as u64,
            },
        )
        .expect("Failed to start thread.");

        Self { cap, _entry: entry }
    }

    pub fn join(&self) -> Result<(), syscall::ZError> {
        syscall::syscall(
            syscall::kZionThreadWait,
            &syscall::ZThreadWaitReq {
                thread_cap: self.cap,
            },
        )
    }
}
