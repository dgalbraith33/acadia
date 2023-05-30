add_executable(zion
  boot/boot_info.cpp
  common/gdt.cpp
  common/load_gdt.s
  debug/debug.cpp
  interrupt/interrupt.cpp
  interrupt/interrupt_enter.s
  interrupt/timer.cpp
  loader/elf_loader.cpp
  loader/init_loader.cpp
  memory/kernel_heap.cpp
  memory/paging_util.cpp
  memory/physical_memory.cpp
  scheduler/context_switch.s
  scheduler/jump_user_space.s
  scheduler/process.cpp
  scheduler/process_manager.cpp
  scheduler/scheduler.cpp
  scheduler/thread.cpp
  syscall/syscall.cpp
  syscall/syscall_enter.s
  zion.cpp)

target_include_directories(zion
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# -c -- Don't run the linker (only necessary for the assembler)
# -ffreestanding
# -fno-rtti -- No runtime type information (might mess with polymorphism?)
# -fno-exceptions -- Disable exceptions.
# -nostdlib -- Don't include the standard library.
# -mabi=sysv -- Explicitly specify the ABI since we will rely on it.
# -mno-red-zone -- Don't put data below the stack pointer (clobbered by interrupts).
# -mcmodel=kernel -- Assume the kernel code is running in the higher half.
# -mgeneral-regs-only -- Prevent GCC from using a whole host of nonsense registers (that we have to enable).
set(_Z_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -c -ffreestanding -fno-rtti -fno-exceptions -nostdlib -mabi=sysv -mno-red-zone -mcmodel=kernel -mgeneral-regs-only")

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



add_library(zion_lib STATIC
  usr/crt0.s
  usr/zcall.cpp)

target_include_directories(zion_lib
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(zion_lib
  PROPERTIES
  COMPILE_FLAGS "${_Z_COMPILE_FLAGS}")
