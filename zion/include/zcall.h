#pragma once

#include <stdint.h>

#include "zcall_macros.h"
#include "ztypes.h"

z_err_t SysCall1(uint64_t code, const void* req);

SYS1(ProcessExit, uint64_t, code);
SYS5(ProcessSpawn, z_cap_t, proc_cap, z_cap_t, bootstrap_cap, z_cap_t*,
     new_proc_cap, z_cap_t*, new_vmas_cap, z_cap_t*, new_bootstrap_cap);

SYS2(ThreadCreate, z_cap_t, proc_cap, z_cap_t*, thread_cap);
SYS4(ThreadStart, z_cap_t, thread_cap, uint64_t, entry, uint64_t, arg1,
     uint64_t, arg2);
SYS0(ThreadExit);
SYS1(ThreadWait, z_cap_t, thread_cap);

SYS4(AddressSpaceMap, z_cap_t, vmas_cap, uint64_t, vmas_offset, z_cap_t,
     vmmo_cap, uint64_t*, vaddr);

SYS2(MemoryObjectCreate, uint64_t, size, z_cap_t*, vmmo_cap);
SYS3(MemoryObjectCreatePhysical, uint64_t, paddr, uint64_t, size, z_cap_t*,
     vmmo_cap);
SYS3(MemoryObjectCreateContiguous, uint64_t, size, z_cap_t*, vmmo_cap,
     uint64_t*, paddr);

SYS4(MemoryObjectDuplicate, z_cap_t, vmmo_cap, uint64_t, base_offset, uint64_t,
     length, z_cap_t*, new_vmmo_cap);

SYS2(ChannelCreate, z_cap_t*, channel1, z_cap_t*, channel2);
SYS5(ChannelSend, z_cap_t, chan_cap, uint64_t, num_bytes, const void*, data,
     uint64_t, num_caps, z_cap_t*, caps);
SYS5(ChannelRecv, z_cap_t, chan_cap, uint64_t*, num_bytes, void*, data,
     uint64_t*, num_caps, z_cap_t*, caps);

SYS1(PortCreate, z_cap_t*, port_cap);
SYS5(PortSend, z_cap_t, port_cap, uint64_t, num_bytes, const void*, data,
     uint64_t, num_caps, z_cap_t*, caps);
SYS5(PortRecv, z_cap_t, port_cap, uint64_t*, num_bytes, void*, data, uint64_t*,
     num_caps, z_cap_t*, caps);
SYS5(PortPoll, z_cap_t, port_cap, uint64_t*, num_bytes, void*, data, uint64_t*,
     num_caps, z_cap_t*, caps);

SYS2(IrqRegister, uint64_t, irq_num, z_cap_t*, port_cap);

SYS1(EndpointCreate, z_cap_t*, endpoint_cap);
SYS6(EndpointSend, z_cap_t, endpoint_cap, uint64_t, num_bytes, const void*,
     data, uint64_t, num_caps, const z_cap_t*, caps, z_cap_t*, reply_port_cap);
SYS6(EndpointRecv, z_cap_t, endpoint_cap, uint64_t*, num_bytes, void*, data,
     uint64_t*, num_caps, z_cap_t*, caps, z_cap_t*, reply_port_cap);
SYS5(ReplyPortSend, z_cap_t, reply_port_cap, uint64_t, num_bytes, const void*,
     data, uint64_t, num_caps, z_cap_t*, caps);
SYS5(ReplyPortRecv, z_cap_t, reply_port_cap, uint64_t*, num_bytes, void*, data,
     uint64_t*, num_caps, z_cap_t*, caps);

SYS3(CapDuplicate, z_cap_t, cap_in, z_perm_t, perm_mask, z_cap_t*, cap_out);
SYS1(CapRelease, z_cap_t, cap);

SYS1(MutexCreate, z_cap_t*, mutex_cap);
SYS1(MutexLock, z_cap_t, mutex_cap);
SYS1(MutexRelease, z_cap_t, mutex_cap);

SYS1(Debug, const char*, message);
