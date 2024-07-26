extern crate alloc;

use alloc::string::String;
use alloc::vec::Vec;
use core::ffi::c_void;
use core::fmt;
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

pub struct Writer {
    int_vec: Vec<u8>,
}

impl Writer {
    pub fn new() -> Self {
        Self {
            int_vec: Vec::new(),
        }
    }
}

impl Into<String> for Writer {
    fn into(self) -> String {
        String::from_utf8(self.int_vec).expect("Failed to convert")
    }
}

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for c in s.bytes() {
            self.int_vec.push(c);
        }
        fmt::Result::Ok(())
    }
}

#[macro_export]
macro_rules! debug {
    () => {
        debug("");
    };
    ($fmt:literal) => {
        debug($fmt);
    };
    ($fmt:literal, $($val:expr),+) => {{
        use core::fmt::Write as _;
        use alloc::string::String;
        let mut w = mammoth::syscall::Writer::new();
        write!(&mut w, $fmt, $($val),*).expect("Failed to format");
        let s: String = w.into();
        debug(&s);
    }};
}
