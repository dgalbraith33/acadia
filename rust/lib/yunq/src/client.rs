use crate::buffer::ByteBuffer;
use crate::message::YunqMessage;
use alloc::vec::Vec;
use mammoth::cap::Capability;
use mammoth::zion::ZError;

pub fn call_endpoint<Req: YunqMessage, Resp: YunqMessage, const N: usize>(
    request_id: u64,
    req: &Req,
    byte_buffer: &mut ByteBuffer<N>,
    endpoint_cap: &Capability,
) -> Result<Resp, ZError> {
    let mut cap_buffer = Vec::new();
    let length = req.serialize_as_request(request_id, byte_buffer, &mut cap_buffer)?;

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

    let resp_code: u64 = byte_buffer.at(8)?;

    if resp_code != 0 {
        return Err(ZError::from(resp_code));
    }

    Ok(Resp::parse_from_request(&byte_buffer, &cap_buffer)?)
}
