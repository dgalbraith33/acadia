use crate::syscall;
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
