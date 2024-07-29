use crate::buffer::ByteBuffer;
use crate::message::YunqMessage;
use alloc::vec::Vec;
use core::ffi::c_void;
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

    let mut reply_port_cap: u64 = 0;
    let send_req = mammoth::zion::ZEndpointSendReq {
        caps: cap_buffer.as_ptr(),
        num_caps: cap_buffer.len() as u64,
        endpoint_cap,
        data: byte_buffer.raw_ptr() as *const c_void,
        num_bytes: 16 + length as u64,
        reply_port_cap: &mut reply_port_cap as *mut z_cap_t,
    };

    mammoth::syscall::syscall(mammoth::zion::kZionEndpointSend, &send_req)?;
    // FIXME: Add a way to zero out the byte buffer.

    let mut buffer_size = byte_buffer.size();
    let mut num_caps: u64 = 10;
    cap_buffer = vec![0; num_caps as usize];
    let recv_req = mammoth::zion::ZReplyPortRecvReq {
        reply_port_cap,
        caps: cap_buffer.as_mut_ptr(),
        num_caps: &mut num_caps as *mut u64,
        data: byte_buffer.mut_ptr() as *mut c_void,
        num_bytes: &mut buffer_size as *mut u64,
    };

    mammoth::syscall::syscall(mammoth::zion::kZionReplyPortRecv, &recv_req)?;

    if byte_buffer.at::<u32>(0)? != SENTINEL {
        return Err(ZError::INVALID_RESPONSE);
    }

    let resp_code: u64 = byte_buffer.at(8)?;
    if resp_code != 0 {
        return Err(ZError::from(resp_code));
    }

    Ok(Resp::parse(&byte_buffer, 16, &cap_buffer)?)
}
