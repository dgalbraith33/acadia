use crate::syscall::{cap_duplicate, cap_release, port_create, port_recv};
use crate::zion::{kZionPerm_Read, z_cap_t, ZError};

pub struct PortServer {
    port_cap: z_cap_t,
}

impl PortServer {
    pub fn new() -> Result<Self, ZError> {
        Ok(Self {
            port_cap: port_create()?,
        })
    }

    pub fn create_client_cap(&self) -> Result<z_cap_t, ZError> {
        cap_duplicate(self.port_cap, !kZionPerm_Read)
    }

    pub fn recv_byte(&self) -> Result<u8, ZError> {
        let mut caps: [z_cap_t; 0] = [];
        let mut bytes: [u8; 1] = [0];

        port_recv(self.port_cap, &mut bytes, &mut caps)?;

        Ok(bytes[0])
    }

    pub fn recv_u16(&self) -> Result<u16, ZError> {
        let mut caps: [z_cap_t; 0] = [];
        let mut bytes: [u8; 2] = [0; 2];

        port_recv(self.port_cap, &mut bytes, &mut caps)?;

        Ok(u16::from_le_bytes(bytes))
    }
}

impl Drop for PortServer {
    fn drop(&mut self) {
        cap_release(self.port_cap).expect("Failed to release port cap");
    }
}
