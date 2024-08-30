extern crate alloc;

use crate::cap::Capability;
use crate::zion;
use crate::zion::z_cap_t;
use crate::zion::ZError;
use core::ffi::c_void;

#[cfg(feature = "hosted")]
use core::panic::PanicInfo;

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

#[cfg(feature = "hosted")]
#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    unsafe {
        if crate::mem::CAN_ALLOC {
            crate::debug!("Panic occured: {}", info);
        } else {
            debug("Panic occured before heap initialized.")
        }
    }
    // Internal error.
    let req = zion::ZProcessExitReq { code: 0x100 };
    let _ = syscall(zion::kZionProcessExit, &req);
    unreachable!()
}

pub fn debug(msg: &str) {
    let req = zion::ZDebugReq {
        message: msg.as_ptr() as *const i8,
        size: msg.len() as u64,
    };
    syscall(zion::kZionDebug, &req).expect("Failed to write");
}

pub fn process_spawn(
    proc_cap: &Capability,
    bootstrap_cap: Capability,
) -> Result<(Capability, Capability, u64), ZError> {
    let mut new_proc_cap = 0;
    let mut new_as_cap = 0;
    let mut new_bootstrap_cap = 0;

    syscall(
        zion::kZionProcessSpawn,
        &zion::ZProcessSpawnReq {
            proc_cap: proc_cap.raw(),
            bootstrap_cap: bootstrap_cap.release(),
            new_proc_cap: &mut new_proc_cap,
            new_vmas_cap: &mut new_as_cap,
            new_bootstrap_cap: &mut new_bootstrap_cap,
        },
    )?;

    Ok((
        Capability::take(new_proc_cap),
        Capability::take(new_as_cap),
        new_bootstrap_cap,
    ))
}

pub fn process_exit(code: u64) -> ! {
    let _ = syscall(zion::kZionProcessExit, &zion::ZProcessExitReq { code });

    unreachable!()
}

pub fn process_wait(proc_cap: &Capability) -> Result<u64, ZError> {
    let mut err_code = 0;
    syscall(
        zion::kZionProcessWait,
        &zion::ZProcessWaitReq {
            proc_cap: proc_cap.raw(),
            exit_code: &mut err_code,
        },
    )?;
    Ok(err_code)
}

pub fn thread_create(proc_cap: &Capability) -> Result<Capability, ZError> {
    let mut cap = 0;
    syscall(
        zion::kZionThreadCreate,
        &zion::ZThreadCreateReq {
            proc_cap: proc_cap.raw(),
            thread_cap: &mut cap,
        },
    )?;
    Ok(Capability::take(cap))
}

pub fn thread_sleep(millis: u64) -> Result<(), ZError> {
    syscall(zion::kZionThreadSleep, &zion::ZThreadSleepReq { millis })
}

pub fn thread_start(
    thread_cap: &Capability,
    entry: u64,
    arg1: u64,
    arg2: u64,
) -> Result<(), ZError> {
    syscall(
        zion::kZionThreadStart,
        &zion::ZThreadStartReq {
            thread_cap: thread_cap.raw(),
            entry,
            arg1,
            arg2,
        },
    )
}

pub fn thread_wait(thread_cap: &Capability) -> Result<(), ZError> {
    syscall(
        zion::kZionThreadWait,
        &zion::ZThreadWaitReq {
            thread_cap: thread_cap.raw(),
        },
    )
}

pub fn thread_exit() -> ! {
    let _ = syscall(zion::kZionThreadExit, &zion::ZThreadExitReq {});
    unreachable!();
}

pub fn memory_object_create(size: u64) -> Result<Capability, ZError> {
    let mut vmmo_cap = 0;
    let obj_req = zion::ZMemoryObjectCreateReq {
        size,
        vmmo_cap: &mut vmmo_cap as *mut u64,
    };
    syscall(zion::kZionMemoryObjectCreate, &obj_req)?;
    Ok(Capability::take(vmmo_cap))
}

pub fn memory_object_direct_physical(paddr: u64, size: u64) -> Result<Capability, ZError> {
    let mut vmmo_cap = 0;
    syscall(
        zion::kZionMemoryObjectCreatePhysical,
        &zion::ZMemoryObjectCreatePhysicalReq {
            paddr,
            size,
            vmmo_cap: &mut vmmo_cap,
        },
    )?;
    Ok(Capability::take(vmmo_cap))
}

pub fn memory_object_inspect(mem_cap: &Capability) -> Result<u64, ZError> {
    let mut mem_size = 0;
    syscall(
        zion::kZionMemoryObjectInspect,
        &zion::ZMemoryObjectInspectReq {
            vmmo_cap: mem_cap.raw(),
            size: &mut mem_size,
        },
    )?;
    Ok(mem_size)
}

pub fn memory_obj_duplicate(
    mem_cap: &Capability,
    base_offset: u64,
    length: u64,
) -> Result<Capability, ZError> {
    let mut new_cap = 0;
    syscall(
        zion::kZionMemoryObjectDuplicate,
        &zion::ZMemoryObjectDuplicateReq {
            vmmo_cap: mem_cap.raw(),
            base_offset,
            length,
            new_vmmo_cap: &mut new_cap,
        },
    )?;
    Ok(Capability::take(new_cap))
}

pub fn address_space_map(vmmo_cap: &Capability) -> Result<u64, ZError> {
    let mut vaddr: u64 = 0;
    // FIXME: Allow caller to pass these options.
    let vmas_req = zion::ZAddressSpaceMapReq {
        vmmo_cap: vmmo_cap.raw(),
        vmas_cap: unsafe { crate::init::SELF_VMAS_CAP },
        align: 0x2000,
        vaddr: &mut vaddr as *mut u64,
        vmas_offset: 0,
    };

    syscall(zion::kZionAddressSpaceMap, &vmas_req)?;
    Ok(vaddr)
}

pub fn address_space_map_external(
    vmas_cap: &Capability,
    vmmo_cap: &Capability,
    vaddr: u64,
) -> Result<(), ZError> {
    let mut vaddr_throw: u64 = 0;
    let vmas_req = zion::ZAddressSpaceMapReq {
        vmas_cap: vmas_cap.raw(),
        vmmo_cap: vmmo_cap.raw(),
        align: 0,
        vaddr: &mut vaddr_throw as *mut u64,
        vmas_offset: vaddr,
    };

    syscall(zion::kZionAddressSpaceMap, &vmas_req)?;
    Ok(())
}

pub fn address_space_unmap(lower_addr: u64, upper_addr: u64) -> Result<(), ZError> {
    syscall(
        zion::kZionAddressSpaceUnmap,
        &zion::ZAddressSpaceUnmapReq {
            vmas_cap: unsafe { crate::init::SELF_VMAS_CAP },
            lower_addr,
            upper_addr,
        },
    )
}

pub fn port_create() -> Result<Capability, ZError> {
    let mut port_cap = 0;
    syscall(
        zion::kZionPortCreate,
        &zion::ZPortCreateReq {
            port_cap: &mut port_cap,
        },
    )?;
    Ok(Capability::take(port_cap))
}

pub fn port_send(port_cap: &Capability, bytes: &[u8], caps: &mut [z_cap_t]) -> Result<(), ZError> {
    syscall(
        zion::kZionPortSend,
        &zion::ZPortSendReq {
            port_cap: port_cap.raw(),
            num_bytes: bytes.len() as u64,
            data: bytes.as_ptr() as *const c_void,
            num_caps: caps.len() as u64,
            // FIXME: This shouldn't need to be mutable.
            caps: caps.as_mut_ptr(),
        },
    )
}

pub fn port_recv(
    port_cap: &Capability,
    bytes: &mut [u8],
    caps: &mut [u64],
) -> Result<(u64, u64), ZError> {
    let mut num_bytes = bytes.len() as u64;
    let mut num_caps = caps.len() as u64;
    syscall(
        zion::kZionPortRecv,
        &zion::ZPortRecvReq {
            port_cap: port_cap.raw(),
            data: bytes.as_mut_ptr() as *mut c_void,
            num_bytes: &mut num_bytes as *mut u64,
            caps: caps.as_mut_ptr(),
            num_caps: &mut num_caps as *mut u64,
        },
    )?;
    Ok((num_bytes, num_caps))
}

pub fn port_poll(
    port_cap: &Capability,
    bytes: &mut [u8],
    caps: &mut [u64],
) -> Result<(u64, u64), ZError> {
    let mut num_bytes = bytes.len() as u64;
    let mut num_caps = caps.len() as u64;
    let req = zion::ZPortPollReq {
        port_cap: port_cap.raw(),
        data: bytes.as_mut_ptr() as *mut c_void,
        num_bytes: &mut num_bytes as *mut u64,
        caps: caps.as_mut_ptr(),
        num_caps: &mut num_caps as *mut u64,
    };
    syscall(zion::kZionPortPoll, &req)?;
    Ok((num_bytes, num_caps))
}

pub fn endpoint_create() -> Result<Capability, ZError> {
    let mut endpoint_cap: z_cap_t = 0;
    syscall(
        zion::kZionEndpointCreate,
        &zion::ZEndpointCreateReq {
            endpoint_cap: &mut endpoint_cap,
        },
    )?;
    Ok(Capability::take(endpoint_cap))
}

pub fn endpoint_send(
    endpoint_cap: &Capability,
    bytes: &[u8],
    caps: &[z_cap_t],
) -> Result<Capability, ZError> {
    let mut reply_port_cap: u64 = 0;
    let send_req = zion::ZEndpointSendReq {
        caps: caps.as_ptr(),
        num_caps: caps.len() as u64,
        endpoint_cap: endpoint_cap.raw(),
        data: bytes.as_ptr() as *const c_void,
        num_bytes: bytes.len() as u64,
        reply_port_cap: &mut reply_port_cap,
    };

    syscall(zion::kZionEndpointSend, &send_req)?;

    Ok(Capability::take(reply_port_cap))
}

pub fn endpoint_recv(
    endpoint_cap: &Capability,
    bytes: &mut [u8],
    caps: &mut [z_cap_t],
) -> Result<(u64, u64, Capability), ZError> {
    let mut num_bytes = bytes.len() as u64;
    let mut num_caps = caps.len() as u64;
    let mut reply_port_cap = 0;
    let recv_req = zion::ZEndpointRecvReq {
        endpoint_cap: endpoint_cap.raw(),
        data: bytes.as_mut_ptr() as *mut c_void,
        num_bytes: &mut num_bytes,
        caps: caps.as_mut_ptr(),
        num_caps: &mut num_caps,
        reply_port_cap: &mut reply_port_cap,
    };

    syscall(zion::kZionEndpointRecv, &recv_req)?;

    Ok((num_bytes, num_caps, Capability::take(reply_port_cap)))
}

pub fn reply_port_send(
    reply_port_cap: Capability,
    bytes: &[u8],
    caps: &[z_cap_t],
) -> Result<(), ZError> {
    syscall(
        zion::kZionReplyPortSend,
        &zion::ZReplyPortSendReq {
            reply_port_cap: reply_port_cap.raw(),
            data: bytes.as_ptr() as *const c_void,
            num_bytes: bytes.len() as u64,
            caps: caps.as_ptr(),
            num_caps: caps.len() as u64,
        },
    )
}

pub fn reply_port_recv(
    reply_port_cap: Capability,
    bytes: &mut [u8],
    caps: &mut [z_cap_t],
) -> Result<(u64, u64), ZError> {
    let mut num_bytes = bytes.len() as u64;
    let mut num_caps = caps.len() as u64;
    let recv_req = zion::ZReplyPortRecvReq {
        reply_port_cap: reply_port_cap.raw(),
        caps: caps.as_mut_ptr(),
        num_caps: &mut num_caps,
        data: bytes.as_mut_ptr() as *mut c_void,
        num_bytes: &mut num_bytes,
    };

    syscall(zion::kZionReplyPortRecv, &recv_req)?;

    Ok((num_bytes, num_caps))
}

pub fn semaphore_create() -> Result<Capability, ZError> {
    let mut sem_cap: z_cap_t = 0;
    syscall(
        zion::kZionSemaphoreCreate,
        &zion::ZSemaphoreCreateReq {
            semaphore_cap: &mut sem_cap,
        },
    )?;

    Ok(Capability::take(sem_cap))
}

pub fn semaphone_signal(sem_cap: &Capability) -> Result<(), ZError> {
    syscall(
        zion::kZionSemaphoreSignal,
        &zion::ZSemaphoreSignalReq {
            semaphore_cap: sem_cap.raw(),
        },
    )
}

pub fn semaphone_wait(sem_cap: &Capability) -> Result<(), ZError> {
    syscall(
        zion::kZionSemaphoreWait,
        &zion::ZSemaphoreWaitReq {
            semaphore_cap: sem_cap.raw(),
        },
    )
}
