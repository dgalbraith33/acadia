use crate::buffer::ByteBuffer;
use alloc::vec::Vec;
use mammoth::zion::z_cap_t;
use mammoth::zion::ZError;

pub const MESSAGE_IDENT: u32 = 0x33441122;
const SENTINEL: u32 = 0xBEEFDEAD;
pub const MESSAGE_HEADER_SIZE: usize = 24; // 4x uint32, 1x uint64

pub fn field_offset(offset: usize, field_index: usize) -> usize {
    offset + MESSAGE_HEADER_SIZE + (8 * field_index)
}

pub trait YunqMessage {
    fn parse<const N: usize>(
        buf: &ByteBuffer<N>,
        offset: usize,
        caps: &Vec<z_cap_t>,
    ) -> Result<Self, ZError>
    where
        Self: Sized;

    fn parse_from_request<const N: usize>(
        buf: &ByteBuffer<N>,
        caps: &Vec<z_cap_t>,
    ) -> Result<Self, ZError>
    where
        Self: Sized,
    {
        if buf.at::<u32>(0)? != SENTINEL {
            return Err(ZError::INVALID_RESPONSE);
        }

        let resp_code: u64 = buf.at(8)?;
        if resp_code != 0 {
            return Err(ZError::from(resp_code));
        }

        Ok(Self::parse(&buf, 16, &caps)?)
    }

    fn serialize<const N: usize>(
        &self,
        buf: &mut ByteBuffer<N>,
        offset: usize,
        caps: &mut Vec<z_cap_t>,
    ) -> Result<usize, ZError>;

    fn serialize_as_request<const N: usize>(
        &self,
        request_id: u64,
        buf: &mut ByteBuffer<N>,
        caps: &mut Vec<z_cap_t>,
    ) -> Result<usize, ZError> {
        buf.write_at(0, SENTINEL)?;
        buf.write_at(8, request_id as u64)?;

        let length = self.serialize(buf, 16, caps)?;

        buf.write_at(4, (16 + length) as u32)?;

        Ok(length + 16)
    }
}

pub struct Empty {}

impl YunqMessage for Empty {
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

    fn serialize<const N: usize>(
        &self,
        _buf: &mut ByteBuffer<N>,
        _offset: usize,
        _caps: &mut Vec<z_cap_t>,
    ) -> Result<usize, ZError> {
        todo!()
    }
}
