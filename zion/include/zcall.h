#pragma once

#include <stdint.h>

#include "ztypes.h"

#define SYS0(name)                         \
  struct Z##name##Req {};                  \
  [[nodiscard]] inline z_err_t Z##name() { \
    Z##name##Req req{};                    \
    return SysCall1(kZion##name, &req);    \
  }

#define SYS1(name, t1, a1)                      \
  struct Z##name##Req {                         \
    t1 a1;                                      \
  };                                            \
  [[nodiscard]] inline z_err_t Z##name(t1 a1) { \
    Z##name##Req req{                           \
        .a1 = a1,                               \
    };                                          \
    return SysCall1(kZion##name, &req);         \
  }

#define SYS2(name, t1, a1, t2, a2)                     \
  struct Z##name##Req {                                \
    t1 a1;                                             \
    t2 a2;                                             \
  };                                                   \
  [[nodiscard]] inline z_err_t Z##name(t1 a1, t2 a2) { \
    Z##name##Req req{                                  \
        .a1 = a1,                                      \
        .a2 = a2,                                      \
    };                                                 \
    return SysCall1(kZion##name, &req);                \
  }

#define SYS3(name, t1, a1, t2, a2, t3, a3)                    \
  struct Z##name##Req {                                       \
    t1 a1;                                                    \
    t2 a2;                                                    \
    t3 a3;                                                    \
  };                                                          \
  [[nodiscard]] inline z_err_t Z##name(t1 a1, t2 a2, t3 a3) { \
    Z##name##Req req{                                         \
        .a1 = a1,                                             \
        .a2 = a2,                                             \
        .a3 = a3,                                             \
    };                                                        \
    return SysCall1(kZion##name, &req);                       \
  }

#define SYS4(name, t1, a1, t2, a2, t3, a3, t4, a4)                   \
  struct Z##name##Req {                                              \
    t1 a1;                                                           \
    t2 a2;                                                           \
    t3 a3;                                                           \
    t4 a4;                                                           \
  };                                                                 \
  [[nodiscard]] inline z_err_t Z##name(t1 a1, t2 a2, t3 a3, t4 a4) { \
    Z##name##Req req{                                                \
        .a1 = a1,                                                    \
        .a2 = a2,                                                    \
        .a3 = a3,                                                    \
        .a4 = a4,                                                    \
    };                                                               \
    return SysCall1(kZion##name, &req);                              \
  }

#define SYS5(name, t1, a1, t2, a2, t3, a3, t4, a4, t5, a5)                  \
  struct Z##name##Req {                                                     \
    t1 a1;                                                                  \
    t2 a2;                                                                  \
    t3 a3;                                                                  \
    t4 a4;                                                                  \
    t5 a5;                                                                  \
  };                                                                        \
  [[nodiscard]] inline z_err_t Z##name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5) { \
    Z##name##Req req{                                                       \
        .a1 = a1,                                                           \
        .a2 = a2,                                                           \
        .a3 = a3,                                                           \
        .a4 = a4,                                                           \
        .a5 = a5,                                                           \
    };                                                                      \
    return SysCall1(kZion##name, &req);                                     \
  }

z_err_t SysCall1(uint64_t code, const void* req);

SYS1(ProcessExit, uint64_t, code);
SYS5(ProcessSpawn, z_cap_t, proc_cap, z_cap_t, bootstrap_cap, z_cap_t*,
     new_proc_cap, z_cap_t*, new_vmas_cap, z_cap_t*, new_bootstrap_cap);

SYS2(ThreadCreate, z_cap_t, proc_cap, z_cap_t*, thread_cap);
SYS4(ThreadStart, z_cap_t, thread_cap, uint64_t, entry, uint64_t, arg1,
     uint64_t, arg2);
SYS0(ThreadExit);

SYS4(AddressSpaceMap, z_cap_t, vmas_cap, uint64_t, vmas_offset, z_cap_t,
     vmmo_cap, uint64_t*, vaddr);

SYS2(MemoryObjectCreate, uint64_t, size, z_cap_t*, vmmo_cap);
SYS3(MemoryObjectCreatePhysical, uint64_t, paddr, uint64_t, size, z_cap_t*,
     vmmo_cap);
SYS3(MemoryObjectCreateContiguous, uint64_t, size, z_cap_t*, vmmo_cap,
     uint64_t*, paddr);
SYS2(TempPcieConfigObjectCreate, z_cap_t*, vmmo_cap, uint64_t*, vmmo_size);

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
