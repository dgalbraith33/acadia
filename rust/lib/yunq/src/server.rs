use crate::buffer::ByteBuffer;
use alloc::vec::Vec;
use mammoth::syscall;
use mammoth::zion::z_cap_t;
use mammoth::zion::ZError;

pub trait YunqServer {
    fn server_loop(&self) {
        loop {
            let mut byte_buffer = ByteBuffer::<1024>::new();
            let mut cap_buffer = vec![0; 10];
            let (_, _, reply_port_cap) = syscall::endpoint_recv(
                self.endpoint_cap(),
                byte_buffer.mut_slice(),
                &mut cap_buffer,
            )
            .expect("Failed to call endpoint recv");

            let method = byte_buffer
                .at::<u64>(8)
                .expect("Failed to access request length.");
            let resp = self.handle_request(method, &mut byte_buffer, &mut cap_buffer);
            match resp {
                Ok(resp_len) => syscall::reply_port_send(
                    reply_port_cap,
                    byte_buffer.slice(resp_len),
                    &cap_buffer,
                )
                .expect("Failed to reply"),
                Err(err) => {
                    crate::message::serialize_error(&mut byte_buffer, err);
                    syscall::reply_port_send(reply_port_cap, &byte_buffer.slice(0x10), &[])
                        .expect("Failed to reply w/ error")
                }
            }
        }
    }

    fn endpoint_cap(&self) -> z_cap_t;
    fn handle_request(
        &self,
        method_number: u64,
        byte_buffer: &mut ByteBuffer<1024>,
        cap_buffer: &mut Vec<z_cap_t>,
    ) -> Result<usize, ZError>;
}
