include!("bindings.rs");

use core::fmt;

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
        let str = match self {
            ZError::INVALID_ARGUMENT => "INVALID_ARGUMENT",
            ZError::BUFFER_SIZE => "BUFFER_SIZE",
            ZError::INTERNAL => "INTERNAL",
            ZError::UNIMPLEMENTED => "UNIMPLEMENTED",
            ZError::INVALID_RESPONSE => "INVALID_RESPONSE",
            ZError::CAP_NOT_FOUND => "CAP_NOT_FOUND",
            ZError::CAP_WRONG_TYPE => "CAP_WRONG_TYPE",
            ZError::CAP_PERMISSION_DENIED => "CAP_PERMISSION_DENIED",
            _ => "ZError",
        };
        f.write_str(str)
    }
}
