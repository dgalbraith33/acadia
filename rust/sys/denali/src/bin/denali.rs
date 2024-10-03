#![no_std]
#![no_main]

extern crate alloc;

use mammoth::{define_entry, zion::z_err_t};

use denali::ahci::AhciController;

define_entry!();

#[no_mangle]
extern "C" fn main() -> z_err_t {
    mammoth::debug!("IN Denali!");

    let yellowstone = yellowstone_yunq::from_init_endpoint();

    let ahci_info = yellowstone
        .get_ahci_info()
        .expect("Failed to get ahci info");

    let ahci_controller = AhciController::new(
        mammoth::mem::MemoryRegion::from_cap(mammoth::cap::Capability::take(ahci_info.ahci_region))
            .unwrap(),
    );

    ahci_controller.join().unwrap();

    0
}
