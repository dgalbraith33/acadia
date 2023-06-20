.global syscall_enter
syscall_enter:
  # Technically don't need to save all of these as
  # the SYS V ABI will preserve some of them by
  # default but I doubt that this costs us much.
  push %rbx
  push %rcx # Special! This is the return address
  push %rdx
  push %rsi
  push %rdi
  push %r8
  push %r9
  push %r10
  push %r11
  push %r12
  push %r13
  push %r14
  push %r15

  call GetKernelRsp 

  # RAX holds the kernel RSP now.
  mov %rsp, %rbx
  mov %rax, %rsp
  push %rbx
  
  # Now that we are on the kernel stack we can re-enable interrupts.
  sti

  # Restore caller registers using the userspace rsp in rbx
  mov 0x40(%rbx), %rdi
  mov 0x48(%rbx), %rsi
  # Don't push the rbp and rsp as the callee will do so.
  call SyscallHandler

  # Clear interrupts since we are moving back to the user stack here.
  # The sysret call will re-enable them for us.
  cli

  # Pop the userspace rsp off the stack
  pop %rsp

  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %r11 # Contains rflags.
  pop %r10
  pop %r9
  pop %r8
  pop %rdi
  pop %rsi
  pop %rdx
  pop %rcx # Contains return address.
  pop %rbx
  # Because we haven't touched rax since calling syscall_handler it should still have the return value.
  sysretq

