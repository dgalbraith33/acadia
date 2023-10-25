#pragma once
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

#define SYS6(name, t1, a1, t2, a2, t3, a3, t4, a4, t5, a5, t6, a6)        \
  struct Z##name##Req {                                                   \
    t1 a1;                                                                \
    t2 a2;                                                                \
    t3 a3;                                                                \
    t4 a4;                                                                \
    t5 a5;                                                                \
    t6 a6;                                                                \
  };                                                                      \
  [[nodiscard]] inline z_err_t Z##name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
                                       t6 a6) {                           \
    Z##name##Req req{                                                     \
        .a1 = a1,                                                         \
        .a2 = a2,                                                         \
        .a3 = a3,                                                         \
        .a4 = a4,                                                         \
        .a5 = a5,                                                         \
        .a6 = a6,                                                         \
    };                                                                    \
    return SysCall1(kZion##name, &req);                                   \
  }
