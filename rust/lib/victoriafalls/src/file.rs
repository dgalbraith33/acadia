use crate::OpenFileRequest;
use alloc::string::ToString;
use mammoth::{cap::Capability, mem::MemoryRegion, zion::ZError};

pub struct File {
    memory: MemoryRegion,
    len: usize,
}

impl File {
    pub fn open(path: &str) -> Result<Self, ZError> {
        let vfs = crate::get_client();
        let resp = vfs.open_file(&OpenFileRequest {
            path: path.to_string(),
        })?;

        Ok(Self {
            memory: mammoth::mem::MemoryRegion::from_cap(Capability::take(resp.memory))?,
            len: resp.size as usize,
        })
    }

    pub fn slice(&self) -> &[u8] {
        &self.memory.slice()[..self.len]
    }

    pub fn memory(&self) -> &MemoryRegion {
        &self.memory
    }
}
