use crate::OpenFileRequest;
use alloc::string::ToString;
use mammoth::zion::ZError;

pub struct File {
    memory: mammoth::mem::MemoryRegion,
}

impl File {
    pub fn open(path: &str) -> Result<Self, ZError> {
        let vfs = crate::get_client();
        let resp = vfs.open_file(&OpenFileRequest {
            path: path.to_string(),
        })?;

        Ok(Self {
            memory: mammoth::mem::MemoryRegion::from_cap(resp.memory)?,
        })
    }

    pub fn slice(&self, offset: usize, len: usize) -> &[u8] {
        &self.memory.slice()[offset..offset + len]
    }
}
