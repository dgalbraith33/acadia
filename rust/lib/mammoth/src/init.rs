use crate::syscall;
use crate::zion::z_cap_t;

// From /zion/include/ztypes.h
const Z_INIT_SELF_PROC: u64 = 0x4000_0000;
const Z_INIT_SELF_VMAS: u64 = 0x4000_0001;
const Z_INIT_ENDPOINT: u64 = 0x4100_0000;

pub static mut SELF_PROC_CAP: z_cap_t = 0;
pub static mut SELF_VMAS_CAP: z_cap_t = 0;
pub static mut INIT_ENDPOINT: z_cap_t = 0;

pub fn parse_init_port(port_cap: z_cap_t) {
    loop {
        let mut bytes: [u8; 8] = [0; 8];
        let mut caps: [u64; 1] = [0];

        let resp = syscall::port_poll(port_cap, &mut bytes, &mut caps);
        if let Err(_) = resp {
            break;
        }

        let init_sig = u64::from_le_bytes(bytes);

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
