#![no_std]

use core::include;

include!(concat!(env!("OUT_DIR"), "/yunq.rs"));

pub mod dir;
pub mod file;

static mut VFS_CLIENT: Option<VFSClient> = None;

fn get_client() -> &'static mut VFSClient {
    unsafe {
        if let None = VFS_CLIENT {
            let endpoint_cap = yellowstone::from_init_endpoint()
                .get_endpoint(&yellowstone::GetEndpointRequest {
                    endpoint_name: "victoriafalls".to_string(),
                })
                .expect("Failed to get VFS endpoint");

            VFS_CLIENT = Some(VFSClient::new(Capability::take(endpoint_cap.endpoint)));
        }
        VFS_CLIENT.as_mut().unwrap()
    }
}
