.global context_switch
context_switch:
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

  mov %rsp, (%rdi) # Save rsp to the prev task.
  mov (%rsi), %rsp # Load the next task's rsp.

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
