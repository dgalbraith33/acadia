use alloc::{
    string::{String, ToString as _},
    vec::Vec,
};
use mammoth::zion::ZError;

pub fn exists(path: &str) -> Result<bool, ZError> {
    let vfs = crate::get_client();
    let result = vfs.get_directory(&crate::GetDirectoryRequest {
        path: path.to_string(),
    });

    match result {
        Ok(_) => Ok(true),
        Err(ZError::NOT_FOUND) => Ok(false),
        Err(e) => Err(e),
    }
}

pub fn ls(path: &str) -> Result<Vec<String>, ZError> {
    let vfs = crate::get_client();
    let result = vfs.get_directory(&crate::GetDirectoryRequest {
        path: path.to_string(),
    })?;

    Ok(result.filenames.split(',').map(|s| s.to_string()).collect())
}
