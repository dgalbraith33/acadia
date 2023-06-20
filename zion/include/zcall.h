#pragma once

#include <stdint.h>

#include "ztypes.h"

void ZProcessExit(uint64_t code);

[[nodiscard]] z_err_t ZProcessSpawn(z_cap_t proc_cap, z_cap_t bootstrap_cap,
                                    z_cap_t* new_proc_cap,
                                    z_cap_t* new_vmas_cap,
                                    z_cap_t* new_bootstrap_cap);

// UNUSED for now, I think we can get away with just starting a thread.
[[nodiscard]] z_err_t ZProcessStart(z_cap_t proc_cap, z_cap_t thread_cap,
                                    uint64_t entry, uint64_t arg1,
                                    uint64_t arg2);

[[nodiscard]] z_err_t ZThreadCreate(z_cap_t proc_cap, z_cap_t* thread_cap);

[[nodiscard]] z_err_t ZThreadStart(z_cap_t thread_cap, uint64_t entry,
                                   uint64_t arg1, uint64_t arg2);

void ZThreadExit();

[[nodiscard]] z_err_t ZAddressSpaceMap(z_cap_t vmas_cap, uint64_t vmas_offset,
                                       z_cap_t vmmo_cap, uint64_t* vaddr);
[[nodiscard]] z_err_t ZMemoryObjectCreate(uint64_t size, z_cap_t* vmmo_cap);
[[nodiscard]] z_err_t ZMemoryObjectCreatePhysical(uint64_t paddr, uint64_t size,
                                                  z_cap_t* vmmo_cap);
[[nodiscard]] z_err_t ZMemoryObjectCreateContiguous(uint64_t size,
                                                    z_cap_t* vmmo_cap,
                                                    uint64_t* paddr);

[[nodiscard]] z_err_t ZTempPcieConfigObjectCreate(z_cap_t* vmmo_cap,
                                                  uint64_t* vmmo_size);

[[nodiscard]] z_err_t ZChannelCreate(z_cap_t* channel1, z_cap_t* channel2);
[[nodiscard]] z_err_t ZChannelSend(z_cap_t chan_cap, uint64_t num_bytes,
                                   const void* data, uint64_t num_caps,
                                   const z_cap_t* caps);
[[nodiscard]] z_err_t ZChannelRecv(z_cap_t chan_cap, uint64_t num_bytes,
                                   void* data, uint64_t num_caps, z_cap_t* caps,
                                   uint64_t* actual_bytes,
                                   uint64_t* actual_caps);

[[nodiscard]] z_err_t ZPortCreate(z_cap_t* port_cap);
[[nodiscard]] z_err_t ZPortSend(z_cap_t port_cap, uint64_t num_bytes,
                                const void* data, uint64_t num_caps,
                                z_cap_t* caps);
[[nodiscard]] z_err_t ZPortRecv(z_cap_t port_cap, uint64_t num_bytes,
                                void* data, uint64_t num_caps, z_cap_t* caps,
                                uint64_t* actual_bytes, uint64_t* actual_caps);
[[nodiscard]] z_err_t ZPortPoll(z_cap_t port_cap, uint64_t num_bytes,
                                void* data, uint64_t num_caps, z_cap_t* caps,
                                uint64_t* actual_bytes, uint64_t* actual_caps);
[[nodiscard]] z_err_t ZIrqRegister(uint64_t irq_num, z_cap_t* port_cap);

[[nodiscard]] z_err_t ZCapDuplicate(z_cap_t cap_in, z_cap_t* cap_out);

[[nodiscard]] z_err_t ZDebug(const char* message);
