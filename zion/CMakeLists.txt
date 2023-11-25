add_executable(zion
  boot/acpi.cpp
  boot/boot_info.cpp
  capability/capability_table.cpp
  common/cpu.cpp
  common/gdt.cpp
  common/load_gdt.s
  common/msr.cpp
  common/stack_unwind.cpp
  debug/debug.cpp
  interrupt/apic.cpp
  interrupt/apic_timer.cpp
  interrupt/driver_manager.cpp
  interrupt/interrupt.cpp
  interrupt/interrupt_enter.s
  interrupt/timer.cpp
  lib/memory_mapping_tree.cpp
  lib/message_queue.cpp
  loader/init_loader.cpp
  memory/kernel_heap.cpp
  memory/kernel_stack_manager.cpp
  memory/kernel_vmm.cpp
  memory/paging_util.cpp
  memory/physical_memory.cpp
  memory/slab_allocator.cpp
  memory/user_stack_manager.cpp
  object/address_space.cpp
  object/channel.cpp
  object/endpoint.cpp
  object/ipc_object.cpp
  object/memory_object.cpp
  object/mutex.cpp
  object/port.cpp
  object/process.cpp
  object/reply_port.cpp
  object/semaphore.cpp
  object/thread.cpp
  scheduler/cleanup.cpp
  scheduler/context_switch.s
  scheduler/jump_user_space.s
  scheduler/process_manager.cpp
  scheduler/scheduler.cpp
  syscall/address_space.cpp
  syscall/capability.cpp
  syscall/debug.cpp
  syscall/ipc.cpp
  syscall/memory_object.cpp
  syscall/process.cpp
  syscall/synchronization.cpp
  syscall/syscall.cpp
  syscall/syscall_enter.s
  syscall/thread.cpp
  zion.cpp)

target_include_directories(zion
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

target_link_libraries(zion
  glacier_kernel)

# -c -- Don't run the linker (only necessary for the assembler)
# -ffreestanding
# -fno-rtti -- No runtime type information (might mess with polymorphism?)
# -fno-exceptions -- Disable exceptions.
# -nostdlib -- Don't include the standard library.
# -mabi=sysv -- Explicitly specify the ABI since we will rely on it.
# -mno-red-zone -- Don't put data below the stack pointer (clobbered by interrupts).
# -mcmodel=kernel -- Assume the kernel code is running in the higher half.
# -mgeneral-regs-only -- Prevent GCC from using a whole host of nonsense registers (that we have to enable).
set(_Z_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -c -ffreestanding -fno-rtti -fno-exceptions -fno-use-cxa-atexit -nostdlib -mabi=sysv -mno-red-zone -mcmodel=kernel -mgeneral-regs-only")

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



add_library(zion_stub STATIC
  usr/crt0.s
  usr/zcall.cpp)

target_include_directories(zion_stub
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(zion_stub
  PROPERTIES
  COMPILE_FLAGS "${_Z_COMPILE_FLAGS}")
