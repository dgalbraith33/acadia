use crate::buffer::ByteBuffer;
use crate::message::YunqMessage;
use alloc::vec::Vec;
use mammoth::zion::z_cap_t;
use mammoth::zion::ZError;

pub fn call_endpoint<Req: YunqMessage, Resp: YunqMessage, const N: usize>(
    request_id: u64,
    req: &Req,
    byte_buffer: &mut ByteBuffer<N>,
    endpoint_cap: z_cap_t,
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

    Ok(Resp::parse_from_request(&byte_buffer, &cap_buffer)?)
}
