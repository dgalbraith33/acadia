use core::ffi::c_void;

use crate::zion::{self, z_cap_t, ZError};

#[must_use]
fn syscall<T>(id: u64, req: &T) -> Result<(), ZError> {
    unsafe {
        let resp = zion::SysCall1(id, req as *const T as *const c_void);
        if resp != 0 {
            return Err(zion::ZError::from(resp));
        }
    }
    Ok(())
}

pub fn cap_duplicate(cap: z_cap_t, perm_mask: u64) -> Result<z_cap_t, ZError> {
    let mut new_cap = 0;
    syscall(
        zion::kZionCapDuplicate,
        &zion::ZCapDuplicateReq {
            cap_in: cap,
            perm_mask,
            cap_out: &mut new_cap,
        },
    )?;
    Ok(new_cap)
}

pub fn cap_release(cap: z_cap_t) -> Result<(), ZError> {
    syscall(zion::kZionCapRelease, &zion::ZCapReleaseReq { cap })
}
