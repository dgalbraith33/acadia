#![no_std]
#![no_main]

extern crate alloc;

use alloc::string::ToString;
use mammoth::debug;
use mammoth::define_entry;
use mammoth::thread;
use mammoth::zion::z_err_t;
use mammoth::zion::ZError;
use yellowstone::GetEndpointRequest;
use yellowstone::YellowstoneClient;
use yellowstone::YellowstoneServer;
use yunq::server::YunqServer;

define_entry!();

struct YellowstoneImpl {}

impl yellowstone::YellowstoneServerHandler for YellowstoneImpl {
    fn register_endpoint(&self, req: &yellowstone::RegisterEndpointRequest) -> Result<(), ZError> {
        debug!("{}", req.endpoint_name);
        Ok(())
    }

    fn get_endpoint(&self, req: &GetEndpointRequest) -> Result<yellowstone::Endpoint, ZError> {
        debug!("{}", req.endpoint_name);
        Ok(yellowstone::Endpoint {
            endpoint: unsafe { mammoth::init::SELF_PROC_CAP },
        })
    }

    fn get_ahci_info(&self) -> Result<yellowstone::AhciInfo, ZError> {
        todo!()
    }

    fn get_xhci_info(&self) -> Result<yellowstone::XhciInfo, ZError> {
        todo!()
    }

    fn get_framebuffer_info(&self) -> Result<yellowstone::FramebufferInfo, ZError> {
        todo!()
    }

    fn get_denali(&self) -> Result<yellowstone::DenaliInfo, ZError> {
        todo!()
    }
}

#[no_mangle]
pub extern "C" fn main() -> z_err_t {
    debug!("Testing!");

    let mut yellowstone;
    unsafe {
        yellowstone = YellowstoneClient::new(mammoth::init::INIT_ENDPOINT);
    }

    let endpoint = yellowstone
        .get_endpoint(&GetEndpointRequest {
            endpoint_name: "denali".to_string(),
        })
        .expect("Failed to get endpoint");

    debug!("Got endpoint w/ cap: {:#x}", endpoint.endpoint);

    let e: thread::ThreadEntry = |_| {
        debug!("Testing 1 2 3");
    };
    let t = thread::Thread::spawn(e, core::ptr::null()).expect("Failed to spawn thread");

    t.join().expect("Failed to wait.");

    let server = YellowstoneServer::new(YellowstoneImpl {}).expect("Failed to create server");

    let mut yellowstone = YellowstoneClient::new(server.endpoint_cap());

    let t = server.run_server().expect("Failed to start server");

    let endpoint = yellowstone
        .get_endpoint(&GetEndpointRequest {
            endpoint_name: "test".to_string(),
        })
        .expect("Failed to get endpoint");

    debug!("{:#x}", endpoint.endpoint);

    t.join().expect("Failed to wait");

    0
}
