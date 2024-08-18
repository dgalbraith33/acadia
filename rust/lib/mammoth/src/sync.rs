use crate::{cap::Capability, syscall, zion::ZError};

pub struct Semaphore {
    cap: Capability,
}

impl Semaphore {
    pub fn new() -> Result<Self, ZError> {
        syscall::semaphore_create().map(|cap| Self { cap })
    }

    pub fn wait(&self) -> Result<(), ZError> {
        syscall::semaphone_wait(&self.cap)
    }

    pub fn signal(&self) -> Result<(), ZError> {
        syscall::semaphone_signal(&self.cap)
    }
}
