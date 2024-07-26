extern crate alloc;

use alloc::string::String;
use alloc::vec::Vec;
use core::ffi::c_void;
use core::fmt;
use core::panic::PanicInfo;

include!("bindings.rs");

pub enum ZError {
    UNKNOWN = 0x0,
    // First set of error codes generally indicate user errors.
    INVALID_ARGUMENT = 0x1,
    NOT_FOUND = 0x2,
    PERMISSION_DENIED = 0x3,
    NULL_PTR = 0x4,
    EMPTY = 0x5,
    ALREADY_EXISTS = 0x6,
    BUFFER_SIZE = 0x7,
    FAILED_PRECONDITION = 0x8,

    // Second set of error codes generally indicate service errors.
    INTERNAL = 0x100,
    UNIMPLEMENTED = 0x101,
    EXHAUSTED = 0x102,
    INVALID_RESPONSE = 0x103,

    // Kernel specific error codes (relating to capabilities).
    CAP_NOT_FOUND = 0x1000,
    CAP_WRONG_TYPE = 0x1001,
    CAP_PERMISSION_DENIED = 0x1002,
}

impl From<u64> for ZError {
    fn from(value: u64) -> Self {
        match value {
            0x1 => ZError::INVALID_ARGUMENT,
            0x2 => ZError::NOT_FOUND,
            0x3 => ZError::PERMISSION_DENIED,
            0x4 => ZError::NULL_PTR,
            0x5 => ZError::EMPTY,
            0x6 => ZError::ALREADY_EXISTS,
            0x7 => ZError::BUFFER_SIZE,
            0x8 => ZError::FAILED_PRECONDITION,

            0x100 => ZError::INTERNAL,
            0x101 => ZError::UNIMPLEMENTED,
            0x102 => ZError::EXHAUSTED,
            0x103 => ZError::INVALID_RESPONSE,

            0x1000 => ZError::CAP_NOT_FOUND,
            0x1001 => ZError::CAP_WRONG_TYPE,
            0x1002 => ZError::CAP_PERMISSION_DENIED,
            _ => ZError::UNKNOWN,
        }
    }
}

impl fmt::Debug for ZError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str("ZError")
    }
}

#[must_use]
pub fn syscall<T>(id: u64, req: &T) -> Result<(), ZError> {
    unsafe {
        let resp = SysCall1(id, req as *const T as *const c_void);
        if resp != 0 {
            return Err(ZError::from(resp));
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
    let req = ZProcessExitReq { code: 0x100 };
    let _ = syscall(kZionProcessExit, &req);
    unreachable!()
}

pub fn debug(msg: &str) {
    let req = ZDebugReq {
        message: msg.as_ptr() as *const i8,
        size: msg.len() as u64,
    };
    syscall(kZionDebug, &req).expect("Failed to write");
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
        // TODO: Find a way to do this so we don't have to import writer.
        // We can't fully qualify this if we want to use it in this crate.
        let mut w = Writer::new();
        write!(&mut w, $fmt, $($val),*).expect("Failed to format");
        let s: String = w.into();
        debug(&s);
    }};
}
