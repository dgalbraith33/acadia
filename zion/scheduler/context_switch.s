.global context_switch
context_switch:
  # %rdi -> Prev Task RSP address
  # %rsi -> New Task RSP Address
  # %rdx -> Prev Task FXSAVE Address
  # %rcx -> New Task FXRSTOR Address

  push %rax
  push %rcx
  push %rdx
  push %rbx
  push %rbp
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
  mov %cr3, %rax
  push %rax

  // For the sleep thread rdx will
  // be a nullptr.
  test %rdx, %rdx
  jz switch

  fxsave (%rdx)

switch:
  mov %rsp, (%rdi) # Save rsp to the prev task.
  mov (%rsi), %rsp # Load the next task's rsp.

  // For the sleep thread rcx will
  // be a nullptr.
  test %rcx, %rcx
  jz restore

  fxrstor (%rcx)

restore:
  pop %rax
  mov %rax, %cr3
  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %r11
  pop %r10
  pop %r9
  pop %r8
  pop %rdi
  pop %rsi
  pop %rbp
  pop %rbx
  pop %rdx
  pop %rcx
  pop %rax
  retq
