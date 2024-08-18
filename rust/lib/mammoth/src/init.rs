use crate::cap::Capability;
use crate::syscall;
use crate::zion::z_cap_t;

// From /zion/include/ztypes.h
pub const Z_INIT_SELF_PROC: u64 = 0x4000_0000;
pub const Z_INIT_SELF_VMAS: u64 = 0x4000_0001;
pub const Z_INIT_ENDPOINT: u64 = 0x4100_0000;
const Z_BOOT_DENALI_VMMO: u64 = 0x4200_0000;
const Z_BOOT_VICTORIA_FALLS_VMMO: u64 = 0x4200_0001;
const Z_BOOT_PCI_VMMO: u64 = 0x4200_0002;
const Z_BOOT_FRAMEBUFFER_INFO_VMMO: u64 = 0x4200_0003;

pub static mut SELF_PROC_CAP: z_cap_t = 0;
pub static mut SELF_VMAS_CAP: z_cap_t = 0;
pub static mut INIT_ENDPOINT: z_cap_t = 0;

// Boot capabilities, are generally only passed to yellowstone.
pub static mut BOOT_DENALI_VMMO: z_cap_t = 0;
pub static mut BOOT_VICTORIA_FALLS_VMMO: z_cap_t = 0;
pub static mut BOOT_PCI_VMMO: z_cap_t = 0;
pub static mut BOOT_FRAMEBUFFER_INFO_VMMO: z_cap_t = 0;

pub fn parse_init_port(port_cap: z_cap_t) {
    let init_port = Capability::take(port_cap);
    loop {
        let mut bytes: [u8; 8] = [0; 8];
        let mut caps: [u64; 1] = [0];

        let resp = syscall::port_poll(&init_port, &mut bytes, &mut caps);
        if let Err(_) = resp {
            break;
        }

        let init_sig = u64::from_le_bytes(bytes);

        unsafe {
            match init_sig {
                Z_INIT_SELF_PROC => SELF_PROC_CAP = caps[0],
                Z_INIT_SELF_VMAS => SELF_VMAS_CAP = caps[0],
                Z_INIT_ENDPOINT => INIT_ENDPOINT = caps[0],
                Z_BOOT_DENALI_VMMO => BOOT_DENALI_VMMO = caps[0],
                Z_BOOT_VICTORIA_FALLS_VMMO => BOOT_VICTORIA_FALLS_VMMO = caps[0],
                Z_BOOT_PCI_VMMO => BOOT_PCI_VMMO = caps[0],
                Z_BOOT_FRAMEBUFFER_INFO_VMMO => BOOT_FRAMEBUFFER_INFO_VMMO = caps[0],
                _ => syscall::debug("Unknown Cap in Init"),
            }
        }
    }
}
