.global jump_user_space
// rdi - entry point, rsi - user stack
jump_user_space:
  cli
  mov $0x23, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs

  pushq $0x23 # ss
  pushq %rsi
  pushq $0x202  # Bit 9 enables interrupts.
  pushq $0x1B # cs
  pushq %rdi
  mov %rdx, %rdi
  mov %rcx, %rsi
  iretq
