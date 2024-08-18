use crate::cap::Capability;
use crate::syscall;
use crate::zion;

use alloc::boxed::Box;
use core::ffi::c_void;

pub type ThreadEntry = fn(*const c_void) -> ();

#[no_mangle]
extern "C" fn internal_entry_point(thread_ptr: *const Thread, arg1: *const c_void) -> ! {
    let thread: &Thread = unsafe { thread_ptr.as_ref().expect("Failed to unwrap thread ref") };

    (thread.entry)(arg1);

    syscall::thread_exit()
}
// TODO: Add a Drop implementation that kills this thread and drops its capability.
pub struct Thread {
    cap: Capability,
    // This field only exists to ensure that the entry reference will outlive the thread object
    // itself.
    entry: ThreadEntry,
}

impl Thread {
    pub fn spawn(entry: ThreadEntry, arg1: *const c_void) -> Result<Box<Self>, zion::ZError> {
        let proc_cap = Capability::take_copy(unsafe { crate::init::SELF_PROC_CAP })?;
        let cap = syscall::thread_create(&proc_cap)?;
        let thread = Box::new(Self { cap, entry });
        syscall::thread_start(
            &thread.cap,
            internal_entry_point as u64,
            thread.as_ref() as *const Thread as u64,
            arg1 as u64,
        )?;

        Ok(thread)
    }

    pub fn join(&self) -> Result<(), zion::ZError> {
        syscall::thread_wait(&self.cap)
    }
}
