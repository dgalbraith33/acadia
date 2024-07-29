use crate::buffer::ByteBuffer;
use crate::message::YunqMessage;
use alloc::vec::Vec;
use mammoth::zion::z_cap_t;
use mammoth::zion::ZError;

const SENTINEL: u32 = 0xBEEFDEAD;

pub fn call_endpoint<Req: YunqMessage, Resp: YunqMessage, const N: usize>(
    req: &Req,
    byte_buffer: &mut ByteBuffer<N>,
    endpoint_cap: z_cap_t,
) -> Result<Resp, ZError> {
    byte_buffer.write_at(0, SENTINEL)?;
    byte_buffer.write_at(8, 1 as u64)?;

    let mut cap_buffer = Vec::new();

    let length = req.serialize(byte_buffer, 16, &mut cap_buffer)?;

    byte_buffer.write_at(4, (16 + length) as u32)?;

    let reply_port_cap = mammoth::syscall::endpoint_send(
        endpoint_cap,
        byte_buffer.slice(16 + length),
        cap_buffer.as_slice(),
    )?;

    // FIXME: Add a way to zero out the byte buffer.

    cap_buffer = vec![0; 10];
    mammoth::syscall::reply_port_recv(
        reply_port_cap,
        byte_buffer.mut_slice(),
        cap_buffer.as_mut_slice(),
    )?;

    if byte_buffer.at::<u32>(0)? != SENTINEL {
        return Err(ZError::INVALID_RESPONSE);
    }

    let resp_code: u64 = byte_buffer.at(8)?;
    if resp_code != 0 {
        return Err(ZError::from(resp_code));
    }

    Ok(Resp::parse(&byte_buffer, 16, &cap_buffer)?)
}
