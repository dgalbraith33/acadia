use crate::syscall;
use crate::zion::{z_cap_t, ZError};
use alloc::slice;
use linked_list_allocator::LockedHeap;

#[global_allocator]
static ALLOCATOR: LockedHeap = LockedHeap::empty();

pub static mut CAN_ALLOC: bool = false;

pub fn init_heap() {
    // 1 MiB
    let size = 0x10_0000;
    let vmmo_cap = syscall::memory_object_create(size).expect("Failed to create memory object");
    let vaddr = syscall::address_space_map(vmmo_cap).expect("Failed to map memory object");
    unsafe {
        ALLOCATOR.lock().init(vaddr as *mut u8, size as usize);
        CAN_ALLOC = true;
    }
}

pub struct MemoryRegion {
    mem_cap: z_cap_t,
    virt_addr: u64,
    size: u64,
}

impl MemoryRegion {
    pub fn direct_physical(paddr: u64, size: u64) -> Result<Self, ZError> {
        let mem_cap = syscall::memory_object_direct_physical(paddr, size)?;
        let virt_addr = syscall::address_space_map(mem_cap)?;
        Ok(Self {
            mem_cap,
            virt_addr,
            size,
        })
    }

    pub fn from_cap(mem_cap: z_cap_t) -> Result<Self, ZError> {
        let virt_addr = syscall::address_space_map(mem_cap)?;
        let size = syscall::memory_object_inspect(mem_cap)?;
        Ok(Self {
            mem_cap,
            virt_addr,
            size,
        })
    }

    pub fn new(size: u64) -> Result<Self, ZError> {
        let mem_cap = syscall::memory_object_create(size)?;
        let virt_addr = syscall::address_space_map(mem_cap)?;
        Ok(Self {
            mem_cap,
            virt_addr,
            size,
        })
    }

    pub fn slice<T>(&self) -> &[T] {
        unsafe {
            slice::from_raw_parts(
                self.virt_addr as *const T,
                self.size as usize / size_of::<T>(),
            )
        }
    }

    pub fn mut_slice<T>(&self) -> &mut [T] {
        unsafe {
            slice::from_raw_parts_mut(
                self.virt_addr as *mut T,
                self.size as usize / size_of::<T>(),
            )
        }
    }

    pub fn cap(&self) -> z_cap_t {
        self.mem_cap
    }
}

impl Drop for MemoryRegion {
    fn drop(&mut self) {
        // FIXME: We shouldn't have to do this manual adjustment.
        let mut max = self.virt_addr + self.size;
        if (max & 0xFFF) != 0 {
            max += 0x1000 - (max & 0xFFF);
        }
        syscall::address_space_unmap(self.virt_addr, max).expect("Failed to unmap memory");
        syscall::cap_release(self.mem_cap).expect("Failed to release memory cap");
    }
}
