#![no_std]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use core::ffi::c_void;

pub mod mem;
pub mod syscall;

// From /zion/include/ztypes.h
const Z_INIT_SELF_PROC: u64 = 0x4000_0000;
const Z_INIT_SELF_VMAS: u64 = 0x4000_0001;
const Z_INIT_ENDPOINT: u64 = 0x4100_0000;

static mut SELF_PROC_CAP: syscall::zcap = 0;
static mut SELF_VMAS_CAP: syscall::zcap = 0;
pub static mut INIT_ENDPOINT: syscall::zcap = 0;

pub fn parse_init_port(port_cap: syscall::zcap) {
    loop {
        let mut num_bytes: u64 = 8;
        let mut init_sig: u64 = 0;
        let mut caps: [u64; 1] = [0];
        let mut num_caps: u64 = 1;

        let req = syscall::ZPortPollReq {
            port_cap,
            num_bytes: &mut num_bytes as *mut u64,
            data: &mut init_sig as *mut u64 as *mut c_void,
            caps: &mut caps as *mut u64,
            num_caps: &mut num_caps as *mut u64,
        };
        let resp = syscall::syscall(syscall::kZionPortPoll, &req);
        if let Err(_) = resp {
            break;
        }
        unsafe {
            match init_sig {
                Z_INIT_SELF_PROC => SELF_PROC_CAP = caps[0],
                Z_INIT_SELF_VMAS => SELF_VMAS_CAP = caps[0],
                Z_INIT_ENDPOINT => INIT_ENDPOINT = caps[0],
                _ => syscall::debug("Unknown Cap in Init"),
            }
        }
    }
}

#[macro_export]
macro_rules! define_entry {
    () => {
        #[no_mangle]
        pub extern "C" fn _start(init_port: mammoth::syscall::zcap) -> ! {
            extern "C" {
                fn main() -> z_err_t;
            }
            mammoth::parse_init_port(init_port);
            mammoth::mem::init_heap();
            unsafe {
                let err = main();
                let req = mammoth::syscall::ZProcessExitReq { code: err };
                let _ = mammoth::syscall::syscall(mammoth::syscall::kZionProcessExit, &req);
            }
            unreachable!()
        }
    };
}
