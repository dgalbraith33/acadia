use crate::syscall;
use crate::zion;
use crate::zion::z_cap_t;

use core::ffi::c_void;

pub type ThreadEntry = fn(*const c_void) -> ();

#[no_mangle]
extern "C" fn internal_entry_point(entry_ptr: *const ThreadEntry, arg1: *const c_void) -> ! {
    let entry = unsafe { *entry_ptr };

    entry(arg1);

    syscall::thread_exit()
}
// TODO: Add a Drop implementation that kills this thread and drops its capability.
pub struct Thread<'a> {
    cap: z_cap_t,
    // This field only exists to ensure that the entry reference will outlive the thread object
    // itself.
    _entry: &'a ThreadEntry,
}

impl<'a> Thread<'a> {
    pub fn spawn(entry: &'a ThreadEntry, arg1: *const c_void) -> Result<Self, zion::ZError> {
        let proc_cap = unsafe { crate::init::SELF_PROC_CAP };
        let cap = syscall::thread_create(proc_cap)?;

        syscall::thread_start(
            cap,
            internal_entry_point as u64,
            entry as *const ThreadEntry as u64,
            arg1 as u64,
        )?;

        Ok(Self { cap, _entry: entry })
    }

    pub fn join(&self) -> Result<(), zion::ZError> {
        syscall::thread_wait(self.cap)
    }
}
