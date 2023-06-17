#pragma once

#include <stdint.h>

#include "ztypes.h"

void ZProcessExit(uint64_t code);

[[nodiscard]] z_err_t ZProcessSpawn(uint64_t proc_cap, uint64_t bootstrap_cap,
                                    uint64_t* new_proc_cap,
                                    uint64_t* new_vmas_cap,
                                    uint64_t* new_bootstrap_cap);

// UNUSED for now, I think we can get away with just starting a thread.
[[nodiscard]] z_err_t ZProcessStart(uint64_t proc_cap, uint64_t thread_cap,
                                    uint64_t entry, uint64_t arg1,
                                    uint64_t arg2);

[[nodiscard]] z_err_t ZThreadCreate(uint64_t proc_cap, uint64_t* thread_cap);

[[nodiscard]] z_err_t ZThreadStart(uint64_t thread_cap, uint64_t entry,
                                   uint64_t arg1, uint64_t arg2);

void ZThreadExit();

[[nodiscard]] z_err_t ZAddressSpaceMap(uint64_t vmas_cap, uint64_t vmas_offset,
                                       uint64_t vmmo_cap, uint64_t* vaddr);
[[nodiscard]] z_err_t ZMemoryObjectCreate(uint64_t size, uint64_t* vmmo_cap);
[[nodiscard]] z_err_t ZMemoryObjectCreatePhysical(uint64_t paddr, uint64_t size,
                                                  uint64_t* vmmo_cap);
[[nodiscard]] z_err_t ZMemoryObjectCreateContiguous(uint64_t size,
                                                    uint64_t* vmmo_cap,
                                                    uint64_t* paddr);

[[nodiscard]] z_err_t ZTempPcieConfigObjectCreate(uint64_t* vmmo_cap,
                                                  uint64_t* vmmo_size);

[[nodiscard]] z_err_t ZChannelCreate(uint64_t* channel1, uint64_t* channel2);
[[nodiscard]] z_err_t ZChannelSend(uint64_t chan_cap, uint64_t type,
                                   uint64_t num_bytes, const uint8_t* bytes,
                                   uint64_t num_caps, const uint64_t* caps);
[[nodiscard]] z_err_t ZChannelRecv(uint64_t chan_cap, uint64_t num_bytes,
                                   uint8_t* bytes, uint64_t num_caps,
                                   uint64_t* caps, uint64_t* type,
                                   uint64_t* actual_bytes,
                                   uint64_t* actual_caps);

[[nodiscard]] z_err_t ZPortCreate(uint64_t* port_cap);
[[nodiscard]] z_err_t ZPortSend(uint64_t port_cap, uint64_t num_bytes,
                                const uint8_t* bytes, uint64_t num_caps,
                                uint64_t* caps);
[[nodiscard]] z_err_t ZPortRecv(uint64_t port_cap, uint64_t num_bytes,
                                uint8_t* bytes, uint64_t num_caps,
                                uint64_t* caps, uint64_t* type,
                                uint64_t* actual_bytes, uint64_t* actual_caps);
[[nodiscard]] z_err_t ZPortPoll(uint64_t port_cap, uint64_t num_bytes,
                                uint8_t* bytes, uint64_t num_caps,
                                uint64_t* caps, uint64_t* type,
                                uint64_t* actual_bytes, uint64_t* actual_caps);
[[nodiscard]] z_err_t ZIrqRegister(uint64_t irq_num, uint64_t* port_cap);

[[nodiscard]] z_err_t ZCapDuplicate(uint64_t cap_in, uint64_t* cap_out);

[[nodiscard]] z_err_t ZDebug(const char* message);
