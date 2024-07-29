extern crate alloc;

use crate::zion;
use crate::zion::z_cap_t;
use crate::zion::ZError;
use core::ffi::c_void;
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

pub fn process_exit(code: u64) -> ! {
    let _ = syscall(zion::kZionProcessExit, &zion::ZProcessExitReq { code });

    unreachable!()
}

pub fn thread_create(proc_cap: z_cap_t) -> Result<z_cap_t, ZError> {
    let mut cap = 0;
    syscall(
        zion::kZionThreadCreate,
        &zion::ZThreadCreateReq {
            proc_cap,
            thread_cap: &mut cap as *mut z_cap_t,
        },
    )?;
    Ok(cap)
}

pub fn thread_start(thread_cap: z_cap_t, entry: u64, arg1: u64, arg2: u64) -> Result<(), ZError> {
    syscall(
        zion::kZionThreadStart,
        &zion::ZThreadStartReq {
            thread_cap,
            entry,
            arg1,
            arg2,
        },
    )
}

pub fn thread_wait(thread_cap: z_cap_t) -> Result<(), ZError> {
    syscall(zion::kZionThreadWait, &zion::ZThreadWaitReq { thread_cap })
}

pub fn thread_exit() -> ! {
    let _ = syscall(zion::kZionThreadExit, &zion::ZThreadExitReq {});
    unreachable!();
}

pub fn memory_object_create(size: u64) -> Result<z_cap_t, ZError> {
    let mut vmmo_cap = 0;
    let obj_req = zion::ZMemoryObjectCreateReq {
        size,
        vmmo_cap: &mut vmmo_cap as *mut u64,
    };
    syscall(zion::kZionMemoryObjectCreate, &obj_req)?;
    Ok(vmmo_cap)
}

pub fn address_space_map(vmmo_cap: z_cap_t) -> Result<u64, ZError> {
    let mut vaddr: u64 = 0;
    // FIXME: Allow caller to pass these options.
    let vmas_req = zion::ZAddressSpaceMapReq {
        vmmo_cap,
        vmas_cap: unsafe { crate::init::SELF_VMAS_CAP },
        align: 0x2000,
        vaddr: &mut vaddr as *mut u64,
        vmas_offset: 0,
    };

    syscall(zion::kZionAddressSpaceMap, &vmas_req)?;
    Ok(vaddr)
}

pub fn port_poll(
    port_cap: z_cap_t,
    bytes: &mut [u8],
    caps: &mut [u64],
) -> Result<(u64, u64), ZError> {
    let mut num_bytes = bytes.len() as u64;
    let mut num_caps = caps.len() as u64;
    let req = zion::ZPortPollReq {
        port_cap,
        data: bytes.as_mut_ptr() as *mut c_void,
        num_bytes: &mut num_bytes as *mut u64,
        caps: caps.as_mut_ptr(),
        num_caps: &mut num_caps as *mut u64,
    };
    syscall(zion::kZionPortPoll, &req)?;
    Ok((num_bytes, num_caps))
}

pub fn endpoint_send(
    endpoint_cap: z_cap_t,
    bytes: &[u8],
    caps: &[z_cap_t],
) -> Result<z_cap_t, ZError> {
    let mut reply_port_cap: u64 = 0;
    let send_req = zion::ZEndpointSendReq {
        caps: caps.as_ptr(),
        num_caps: caps.len() as u64,
        endpoint_cap,
        data: bytes.as_ptr() as *const c_void,
        num_bytes: bytes.len() as u64,
        reply_port_cap: &mut reply_port_cap as *mut z_cap_t,
    };

    syscall(zion::kZionEndpointSend, &send_req)?;

    Ok(reply_port_cap)
}

pub fn reply_port_recv(
    reply_port_cap: z_cap_t,
    bytes: &mut [u8],
    caps: &mut [z_cap_t],
) -> Result<(u64, u64), ZError> {
    let mut num_bytes = bytes.len() as u64;
    let mut num_caps = caps.len() as u64;
    let recv_req = zion::ZReplyPortRecvReq {
        reply_port_cap,
        caps: caps.as_mut_ptr(),
        num_caps: &mut num_caps as *mut u64,
        data: bytes.as_mut_ptr() as *mut c_void,
        num_bytes: &mut num_bytes as *mut u64,
    };

    syscall(zion::kZionReplyPortRecv, &recv_req)?;

    Ok((num_bytes, num_caps))
}
