#![no_std]

use core::include;

include!(concat!(env!("OUT_DIR"), "/yunq.rs"));

use mammoth::init::INIT_ENDPOINT;

static mut YELLOWSTONE_INIT: Option<YellowstoneClient> = None;

pub fn from_init_endpoint() -> &'static mut YellowstoneClient {
    unsafe {
        if let None = YELLOWSTONE_INIT {
            YELLOWSTONE_INIT = Some(YellowstoneClient::new(INIT_ENDPOINT));
        }

        YELLOWSTONE_INIT.as_mut().unwrap()
    }
}
