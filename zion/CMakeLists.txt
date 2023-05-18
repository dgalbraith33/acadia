add_executable(zion
  zion.cpp)

# -c -- Don't run the linker.
# -nostdlib -- Don't include the standard library.
# -mabi=sysv -- Explicitly specify the ABI since we will rely on it.
# -mno-red-zone -- Don't put data below the stack pointer (clobbered by interrupts).
# -mcmodel=kernel -- Assume the kernel code is running in the higher half.
# -mgeneral-regs-only -- Prevent GCC from using a whole host of nonsense registers (that we have to enable).
# Hopefully preceded by -mgeneral-regs-only
# -mno-80387
# -mno-mmx
# -mno-3dnow
# -mno-sse -mno-sse2
# -MMD -- Something with the preprocessor?
set(_Z_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -nostdlib -mabi=sysv -mno-red-zone -mcmodel=kernel -mgeneral-regs-only")
# -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -MMD

set(_Z_LINK_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

# -lgcc -- Link against gcc internals. Not sure if necessary.
# -nostdlib -- Don't try to link against the stdlib.
# -nostartfiles -- Don't try to link against crt0.s
# -static -- Prevent trying something with shared libraries (probably irrelevant).
# -z max-page-size=0x1000 -- Assume 4 KiB Pages.
set(_Z_LINK_FLAGS "-T ${_Z_LINK_SCRIPT} -lgcc -nostdlib -nostartfiles -static -z max-page-size=0x1000")


# Don't try to dynamically link.
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

set_target_properties(zion
  PROPERTIES
  COMPILE_FLAGS "${_Z_COMPILE_FLAGS}"
  LINK_FLAGS "${_Z_LINK_FLAGS}"
  )
