#![no_std]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use core::ffi::c_void;
use core::panic::PanicInfo;

include!("bindings.rs");

pub fn syscall<T>(id: u64, req: &T) -> u64 {
    unsafe { SysCall1(id, req as *const T as *const c_void) }
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

// From /zion/include/ztypes.h
const Z_INIT_SELF_PROC: u64 = 0x4000_0000;
const Z_INIT_SELF_VMAS: u64 = 0x4000_0001;
const Z_INIT_ENDPOINT: u64 = 0x4100_0000;

static mut SELF_PROC_CAP: zcap = 0;
static mut SELF_VMAS_CAP: zcap = 0;
static mut INIT_ENDPOINT: zcap = 0;

pub fn parse_init_port(port_cap: zcap) {
    loop {
        let mut num_bytes: u64 = 8;
        let mut init_sig: u64 = 0;
        let mut caps: [u64; 1] = [0];
        let mut num_caps: u64 = 1;

        let req = ZPortPollReq {
            port_cap,
            num_bytes: &mut num_bytes as *mut u64,
            data: &mut init_sig as *mut u64 as *mut c_void,
            caps: &mut caps as *mut u64,
            num_caps: &mut num_caps as *mut u64,
        };
        let resp = syscall(kZionPortPoll, &req);
        if resp != 0 {
            break;
        }
        unsafe {
            match init_sig {
                Z_INIT_SELF_PROC => SELF_PROC_CAP = caps[0],
                Z_INIT_SELF_VMAS => SELF_VMAS_CAP = caps[0],
                Z_INIT_ENDPOINT => INIT_ENDPOINT = caps[0],
                _ => debug("Unknown Cap in Init"),
            }
        }
    }
}

#[macro_export]
macro_rules! define_entry {
    () => {
        #[no_mangle]
        pub extern "C" fn _start(init_port: mammoth::zcap) -> ! {
            extern "C" {
                fn main() -> z_err_t;
            }
            mammoth::parse_init_port(init_port);
            unsafe {
                let err = main();
                let req = mammoth::ZProcessExitReq { code: err };
                mammoth::syscall(mammoth::kZionProcessExit, &req);
            }
            unreachable!()
        }
    };
}
