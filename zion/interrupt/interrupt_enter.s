.macro interrupt_enter
  push %rbp
  push %r15
  push %r14
  push %r13
  push %r12
  push %r11
  push %r10
  push %r9
  push %r8
  push %rdi
  push %rsi
  push %rdx
  push %rcx  # (Return Address)
  push %rbx
  push %rax
  mov %cr2, %rax
  push %rax
.endm

.macro interrupt_exit
  add $8, %rsp
  pop %rax
  pop %rbx
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  pop %r8
  pop %r9
  pop %r10
  pop %r11
  pop %r12
  pop %r13
  pop %r14
  pop %r15
  pop %rbp

  add $8, %rsp # Remove error code.
.endm

.macro isr_handler name error_code=0
.global isr_\name
isr_\name:
  .if \error_code
  .else
  push $0 # if we don't have an error code, equalize the stack.
  .endif
  interrupt_enter
  sti
  mov %rsp, %rdi
  call interrupt_\name
  cli
  interrupt_exit
  iretq
.endm

isr_handler divide_by_zero
isr_handler protection_fault,1
isr_handler page_fault,1
isr_handler fpe_fault

isr_handler timer
isr_handler apic_timer

isr_handler pci1
isr_handler pci2
isr_handler pci3
isr_handler pci4

