#![no_std]
#![no_main]

extern crate alloc;

use alloc::vec::Vec;
use mammoth::{
    cap::Capability,
    define_entry, elf,
    init::BOOT_PCI_VMMO,
    mem::MemoryRegion,
    zion::{z_cap_t, z_err_t, ZError},
};
use yellowstone_yunq::YellowstoneServer;
use yunq::server::YunqServer;

mod gpt;
mod pci;
mod server;

define_entry!();

fn spawn_from_vmmo(vmmo_cap: z_cap_t, server_cap: Capability) -> Result<(), ZError> {
    let region = mammoth::mem::MemoryRegion::from_cap(Capability::take(vmmo_cap))?;
    elf::spawn_process_from_elf_and_init(region.slice(), server_cap)?;
    Ok(())
}

#[no_mangle]
extern "C" fn main() -> z_err_t {
    let pci_region = MemoryRegion::from_cap(Capability::take(unsafe { BOOT_PCI_VMMO }))
        .expect("Failed to create PCI region");
    let context = alloc::rc::Rc::new(
        server::YellowstoneServerContext::new(pci_region)
            .expect("Failed to create yellowstone context"),
    );
    let handler = server::YellowstoneServerImpl::new(context.clone());
    let server = YellowstoneServer::new(handler).expect("Couldn't create yellowstone server");

    let server_thread = server.run_server().expect("Failed to run server");

    spawn_from_vmmo(
        unsafe { mammoth::init::BOOT_DENALI_VMMO },
        server
            .create_client_cap()
            .expect("Failed to create client cap for denali"),
    )
    .expect("Failed to spawn denali");

    context.wait_denali().expect("Failed to wait for denali");
    mammoth::debug!("Denali registered.");

    spawn_from_vmmo(
        unsafe { mammoth::init::BOOT_VICTORIA_FALLS_VMMO },
        server.create_client_cap().unwrap(),
    )
    .expect("Failed to spawn victoriafalls");

    context.wait_victoria_falls().unwrap();
    mammoth::debug!("VFS Registered");

    let file = victoriafalls::file::File::open("/init.txt").unwrap();

    let init_files: Vec<_> = core::str::from_utf8(file.slice())
        .unwrap()
        .trim()
        .split('\n')
        .collect();

    mammoth::debug!("Init files: {:?}", init_files);

    for bin_name in init_files {
        let path = "/bin/" + bin_name;
    }

    server_thread.join().expect("Failed to join thread");
    0
}
