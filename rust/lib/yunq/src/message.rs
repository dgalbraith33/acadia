use crate::buffer::ByteBuffer;
use alloc::vec::Vec;
use mammoth::zion::z_cap_t;
use mammoth::zion::ZError;

pub const MESSAGE_IDENT: u32 = 0x33441122;
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

    fn serialize<const N: usize>(
        &self,
        buf: &mut ByteBuffer<N>,
        offset: usize,
        caps: &mut Vec<z_cap_t>,
    ) -> Result<usize, ZError>;
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
