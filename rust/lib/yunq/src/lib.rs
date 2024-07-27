#![no_std]

#[macro_use]
extern crate alloc;

mod buffer;
mod client;
mod message;

use alloc::string::String;
use alloc::vec::Vec;
pub use buffer::ByteBuffer;
use mammoth::syscall::z_cap_t;
use mammoth::syscall::ZError;
pub use message::YunqMessage;

const MESSAGE_HEADER_SIZE: usize = 24; // 4x uint32, 1x uint64

pub struct GetEndpointRequest {
    pub endpoint_name: String,
}

impl YunqMessage for GetEndpointRequest {
    fn serialize<const N: usize>(
        &self,
        buf: &mut buffer::ByteBuffer<N>,
        offset: usize,
        _caps: &mut Vec<z_cap_t>,
    ) -> Result<usize, ZError> {
        let core_size: u32 = (MESSAGE_HEADER_SIZE + 8 * 1) as u32;
        let mut next_extension = core_size;

        let endpoint_name_offset = next_extension;
        let endpoint_name_length = self.endpoint_name.len() as u32;

        buf.write_str_at(offset + next_extension as usize, &self.endpoint_name)?;
        next_extension += endpoint_name_length;

        buf.write_at(field_offset(offset, 0), endpoint_name_offset)?;
        buf.write_at(field_offset(offset, 0) + 4, endpoint_name_length)?;

        buf.write_at(offset + 0, message::MESSAGE_IDENT)?;
        buf.write_at(offset + 4, core_size)?;
        buf.write_at(offset + 8, next_extension)?;
        buf.write_at(offset + 12, 0 as u32)?;
        Ok(next_extension as usize)
    }

    fn parse<const N: usize>(
        _buf: &ByteBuffer<N>,
        _offset: usize,
        _caps: &Vec<z_cap_t>,
    ) -> Result<Self, ZError>
    where
        Self: Sized,
    {
        todo!()
    }
}

pub struct Endpoint {
    pub endpoint: z_cap_t,
}

fn field_offset(offset: usize, field_index: usize) -> usize {
    offset + MESSAGE_HEADER_SIZE + (8 * field_index)
}

impl YunqMessage for Endpoint {
    fn parse<const N: usize>(
        buf: &ByteBuffer<N>,
        offset: usize,
        caps: &Vec<z_cap_t>,
    ) -> Result<Self, ZError> {
        if buf.at::<u32>(offset + 0)? != message::MESSAGE_IDENT {
            return Err(ZError::INVALID_ARGUMENT);
        }
        // TODO: Parse core size.
        // TODO: Parse extension size.
        // TODO: Check CRC32
        // TODO: Parse options.

        let endpoint_ptr = buf.at::<u64>(field_offset(offset, 0))?;
        let endpoint = caps[endpoint_ptr as usize];
        Ok(Endpoint { endpoint })
    }

    fn serialize<const N: usize>(
        &self,
        _buf: &mut buffer::ByteBuffer<N>,
        _offset: usize,
        _caps: &mut Vec<z_cap_t>,
    ) -> Result<usize, ZError> {
        todo!()
    }
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
        client::call_endpoint(req, &mut self.byte_buffer, self.endpoint_cap)
    }
}
