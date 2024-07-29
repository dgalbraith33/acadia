use crate::syscall;
use crate::zion;
use crate::zion::z_cap_t;
use core::ffi::c_void;

// From /zion/include/ztypes.h
const Z_INIT_SELF_PROC: u64 = 0x4000_0000;
const Z_INIT_SELF_VMAS: u64 = 0x4000_0001;
const Z_INIT_ENDPOINT: u64 = 0x4100_0000;

pub static mut SELF_PROC_CAP: z_cap_t = 0;
pub static mut SELF_VMAS_CAP: z_cap_t = 0;
pub static mut INIT_ENDPOINT: z_cap_t = 0;

pub fn parse_init_port(port_cap: z_cap_t) {
    loop {
        let mut num_bytes: u64 = 8;
        let mut init_sig: u64 = 0;
        let mut caps: [u64; 1] = [0];
        let mut num_caps: u64 = 1;

        let req = zion::ZPortPollReq {
            port_cap,
            num_bytes: &mut num_bytes as *mut u64,
            data: &mut init_sig as *mut u64 as *mut c_void,
            caps: &mut caps as *mut u64,
            num_caps: &mut num_caps as *mut u64,
        };
        let resp = syscall::syscall(zion::kZionPortPoll, &req);
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
