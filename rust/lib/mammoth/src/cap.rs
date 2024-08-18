use crate::cap_syscall;
use crate::zion::{z_cap_t, ZError};

pub struct Capability {
    cap: z_cap_t,
}

impl Capability {
    pub fn take(cap: z_cap_t) -> Self {
        Self { cap }
    }

    pub fn take_copy(cap: z_cap_t) -> Result<Self, ZError> {
        Ok(Self::take(cap_syscall::cap_duplicate(
            cap,
            Self::PERMS_ALL,
        )?))
    }

    pub fn raw(&self) -> z_cap_t {
        self.cap
    }

    pub fn release(mut self) -> z_cap_t {
        let cap = self.cap;
        self.cap = 0;
        cap
    }

    pub const PERMS_ALL: u64 = u64::MAX;
    pub fn duplicate(&self, perm_mask: u64) -> Result<Self, ZError> {
        Ok(Self::take(cap_syscall::cap_duplicate(self.cap, perm_mask)?))
    }
}

impl Drop for Capability {
    fn drop(&mut self) {
        if self.cap != 0 {
            if let Err(e) = cap_syscall::cap_release(self.cap) {
                crate::debug!(
                    "WARN: error during cap release for cap {:#x}: {:?}",
                    self.cap,
                    e
                );
            }
        }
    }
}
