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
  pushf  # Can we just push 0 for flags?
  pushq $0x1B # cs
  pushq %rdi
  iretq
