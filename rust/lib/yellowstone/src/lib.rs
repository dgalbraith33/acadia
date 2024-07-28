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
pub struct GetEndpointRequest {
    pub endpoint_name: String,
}

#[derive(YunqMessage)]
pub struct Endpoint {
    pub endpoint: z_cap_t,
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
    pub fn get_endpoint(&mut self, req: &GetEndpointRequest) -> Result<Endpoint, ZError> {
        yunq::client::call_endpoint(req, &mut self.byte_buffer, self.endpoint_cap)
    }
}
