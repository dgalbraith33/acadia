use crate::syscall;
use crate::SELF_VMAS_CAP;
use linked_list_allocator::LockedHeap;

#[global_allocator]
static ALLOCATOR: LockedHeap = LockedHeap::empty();

pub fn init_heap() {
    // 1 MiB
    let size = 0x10_0000;
    let mut vmmo_cap = 0;
    let obj_req = syscall::ZMemoryObjectCreateReq {
        size,
        vmmo_cap: &mut vmmo_cap as *mut u64,
    };
    syscall::checked_syscall(syscall::kZionMemoryObjectCreate, &obj_req);

    unsafe {
        let mut vaddr: u64 = 0;
        let vmas_req = syscall::ZAddressSpaceMapReq {
            vmmo_cap,
            vmas_cap: SELF_VMAS_CAP,
            align: 0x2000,
            vaddr: &mut vaddr as *mut u64,
            vmas_offset: 0,
        };

        syscall::checked_syscall(syscall::kZionAddressSpaceMap, &vmas_req);
        ALLOCATOR.lock().init(vaddr as *mut u8, size as usize);
    }
}
