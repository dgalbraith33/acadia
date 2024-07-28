#![no_std]
extern crate alloc;
use alloc::string::String;
use alloc::string::ToString;
use mammoth::syscall::z_cap_t;
use mammoth::syscall::ZError;
use yunq::ByteBuffer;
use yunq::YunqMessage;
use yunq_derive::YunqMessage;
#[derive(YunqMessage)]
pub struct RegisterEndpointRequest {
    pub endpoint_name: String,
    pub endpoint_capability: z_cap_t,
}
#[derive(YunqMessage)]
pub struct GetEndpointRequest {
    pub endpoint_name: String,
}
#[derive(YunqMessage)]
pub struct Endpoint {
    pub endpoint: z_cap_t,
}
#[derive(YunqMessage)]
pub struct AhciInfo {
    pub ahci_region: z_cap_t,
    pub region_length: u64,
}
#[derive(YunqMessage)]
pub struct XhciInfo {
    pub xhci_region: z_cap_t,
    pub region_length: u64,
}
#[derive(YunqMessage)]
pub struct FramebufferInfo {
    pub address_phys: u64,
    pub width: u64,
    pub height: u64,
    pub pitch: u64,
    pub bpp: u64,
    pub memory_model: u64,
    pub red_mask_size: u64,
    pub red_mask_shift: u64,
    pub green_mask_size: u64,
    pub green_mask_shift: u64,
    pub blue_mask_size: u64,
    pub blue_mask_shift: u64,
}
#[derive(YunqMessage)]
pub struct DenaliInfo {
    pub denali_endpoint: z_cap_t,
    pub device_id: u64,
    pub lba_offset: u64,
}
pub struct YellowstoneClient {
    endpoint_cap: z_cap_t,
    byte_buffer: ByteBuffer<0x1000>,
}
impl YellowstoneClient {
    pub fn new(endpoint_cap: z_cap_t) -> Self {
        Self {
            endpoint_cap,
            byte_buffer: ByteBuffer::new(),
        }
    }
    pub fn register_endpoint(
        &mut self,
        req: &RegisterEndpointRequest,
    ) -> Result<yunq::message::Empty, ZError> {
        yunq::client::call_endpoint(req, &mut self.byte_buffer, self.endpoint_cap)
    }
    pub fn get_endpoint(
        &mut self,
        req: &GetEndpointRequest,
    ) -> Result<Endpoint, ZError> {
        yunq::client::call_endpoint(req, &mut self.byte_buffer, self.endpoint_cap)
    }
    pub fn get_ahci_info(&mut self) -> Result<AhciInfo, ZError> {
        yunq::client::call_endpoint(
            &yunq::message::Empty {},
            &mut self.byte_buffer,
            self.endpoint_cap,
        )
    }
    pub fn get_xhci_info(&mut self) -> Result<XhciInfo, ZError> {
        yunq::client::call_endpoint(
            &yunq::message::Empty {},
            &mut self.byte_buffer,
            self.endpoint_cap,
        )
    }
    pub fn get_framebuffer_info(&mut self) -> Result<FramebufferInfo, ZError> {
        yunq::client::call_endpoint(
            &yunq::message::Empty {},
            &mut self.byte_buffer,
            self.endpoint_cap,
        )
    }
    pub fn get_denali(&mut self) -> Result<DenaliInfo, ZError> {
        yunq::client::call_endpoint(
            &yunq::message::Empty {},
            &mut self.byte_buffer,
            self.endpoint_cap,
        )
    }
}
