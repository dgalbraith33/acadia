use crate::buffer::ByteBuffer;
use alloc::vec::Vec;
use mammoth::zion::z_cap_t;
use mammoth::zion::ZError;

pub const MESSAGE_IDENT: u32 = 0x33441122;
pub const MESSAGE_HEADER_SIZE: usize = 24; // 4x uint32, 1x uint64
const SENTINEL: u32 = 0xBEEFDEAD;
const SERIALIZE_HEADER_SIZE: u32 = 0x10;

pub fn field_offset(offset: usize, field_index: usize) -> usize {
    offset + MESSAGE_HEADER_SIZE + (8 * field_index)
}

pub fn parse_repeated<T: Copy, const N: usize>(
    buf: &ByteBuffer<N>,
    offset: usize,
    len: usize,
) -> Result<Vec<T>, ZError> {
    let mut repeated = Vec::new();
    for i in 0..len {
        repeated.push(buf.at::<T>(offset + (i * size_of::<T>()))?);
    }
    Ok(repeated)
}

pub fn parse_repeated_message<T: YunqMessage, const N: usize>(
    buf: &ByteBuffer<N>,
    mut offset: usize,
    len: usize,
    caps: &Vec<z_cap_t>,
) -> Result<Vec<T>, ZError> {
    let mut repeated = Vec::new();
    for _ in 0..len {
        // FIXME: This is a bad way to get the length.
        let msg_len = buf.at::<u32>(offset + 8)? as usize;
        repeated.push(T::parse(buf, offset, caps)?);
        offset += msg_len;
    }
    Ok(repeated)
}

pub fn serialize_repeated<T: Copy, const N: usize>(
    buf: &mut ByteBuffer<N>,
    offset: usize,
    data: &Vec<T>,
) -> Result<usize, ZError> {
    for i in 0..data.len() {
        buf.write_at(offset + (i * size_of::<T>()), data[i])?;
    }
    Ok(offset + (data.len() * size_of::<T>()))
}

pub fn serialize_repeated_message<T: YunqMessage, const N: usize>(
    buf: &mut ByteBuffer<N>,
    mut offset: usize,
    data: &Vec<T>,
    caps: &mut Vec<z_cap_t>,
) -> Result<usize, ZError> {
    for item in data {
        offset += item.serialize(buf, offset, caps)?;
    }
    Ok(offset)
}

pub fn serialize_error<const N: usize>(buf: &mut ByteBuffer<N>, err: ZError) {
    buf.write_at(0, SENTINEL)
        .expect("Failed to serialize SENTINEL");
    buf.write_at(4, SERIALIZE_HEADER_SIZE)
        .expect("Failed to serialize size");
    buf.write_at(8, err as u64)
        .expect("Failed to serialize error");
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
        Ok(Self {})
    }

    fn serialize<const N: usize>(
        &self,
        _buf: &mut ByteBuffer<N>,
        _offset: usize,
        _caps: &mut Vec<z_cap_t>,
    ) -> Result<usize, ZError> {
        Ok(0)
    }
}
