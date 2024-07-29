use crate::init::SELF_VMAS_CAP;
use crate::syscall;
use crate::zion;
use linked_list_allocator::LockedHeap;

#[global_allocator]
static ALLOCATOR: LockedHeap = LockedHeap::empty();

pub static mut CAN_ALLOC: bool = false;

pub fn init_heap() {
    // 1 MiB
    let size = 0x10_0000;
    let mut vmmo_cap = 0;
    let obj_req = zion::ZMemoryObjectCreateReq {
        size,
        vmmo_cap: &mut vmmo_cap as *mut u64,
    };
    syscall::syscall(zion::kZionMemoryObjectCreate, &obj_req)
        .expect("Failed to create memory object");

    unsafe {
        let mut vaddr: u64 = 0;
        let vmas_req = zion::ZAddressSpaceMapReq {
            vmmo_cap,
            vmas_cap: SELF_VMAS_CAP,
            align: 0x2000,
            vaddr: &mut vaddr as *mut u64,
            vmas_offset: 0,
        };

        syscall::syscall(zion::kZionAddressSpaceMap, &vmas_req)
            .expect("Failed to map memory object");
        ALLOCATOR.lock().init(vaddr as *mut u8, size as usize);
        CAN_ALLOC = true;
    }
}
