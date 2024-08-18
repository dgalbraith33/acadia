use crate::cap::Capability;
use crate::syscall::{port_create, port_recv};
use crate::zion::{kZionPerm_Read, z_cap_t, ZError};

pub struct PortServer {
    port_cap: Capability,
}

impl PortServer {
    pub fn new() -> Result<Self, ZError> {
        Ok(Self {
            port_cap: port_create()?,
        })
    }

    pub fn create_client_cap(&self) -> Result<z_cap_t, ZError> {
        self.port_cap
            .duplicate(!kZionPerm_Read)
            .map(|c| c.release())
    }

    pub fn recv_byte(&self) -> Result<u8, ZError> {
        let mut caps: [z_cap_t; 0] = [];
        let mut bytes: [u8; 1] = [0];

        port_recv(&self.port_cap, &mut bytes, &mut caps)?;

        Ok(bytes[0])
    }

    pub fn recv_u16(&self) -> Result<u16, ZError> {
        let mut caps: [z_cap_t; 0] = [];
        let mut bytes: [u8; 2] = [0; 2];

        port_recv(&self.port_cap, &mut bytes, &mut caps)?;

        Ok(u16::from_le_bytes(bytes))
    }
}

pub struct PortClient {
    port_cap: Capability,
}

impl PortClient {
    pub fn take_from(port_cap: Capability) -> Self {
        Self { port_cap }
    }

    #[warn(unused_results)]
    pub fn write_u64_and_cap(&self, bytes: u64, cap: Capability) -> Result<(), ZError> {
        let mut caps: [z_cap_t; 1] = [cap.release()];
        crate::syscall::port_send(&self.port_cap, &bytes.to_le_bytes(), &mut caps)
    }
}
