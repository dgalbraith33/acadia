use crate::buffer::ByteBuffer;
use alloc::vec::Vec;
use mammoth::syscall::z_cap_t;
use mammoth::syscall::ZError;

pub const MESSAGE_IDENT: u32 = 0x33441122;

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
