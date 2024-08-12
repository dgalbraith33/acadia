use crate::OpenFileRequest;
use crate::VFSClient;
use alloc::string::ToString;
use mammoth::zion::ZError;

static mut VFS_CLIENT: Option<VFSClient> = None;

fn get_client() -> &'static mut VFSClient {
    unsafe {
        if let None = VFS_CLIENT {
            let endpoint_cap = yellowstone::from_init_endpoint()
                .get_endpoint(&yellowstone::GetEndpointRequest {
                    endpoint_name: "victoriafalls".to_string(),
                })
                .expect("Failed to get VFS endpoint");

            VFS_CLIENT = Some(VFSClient::new(endpoint_cap.endpoint));
        }
        VFS_CLIENT.as_mut().unwrap()
    }
}

pub struct File {
    memory: mammoth::mem::MemoryRegion,
}

impl File {
    pub fn open(path: &str) -> Result<Self, ZError> {
        let vfs = get_client();
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
