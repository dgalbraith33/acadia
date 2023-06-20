.text

.global _start
_start:
  call main
  call _exit

_exit:
  // EXIT syscall.
  mov $1, %rdi
  // Return code as a "struct" on the stack.
  push %rax
  mov %rsp, %rsi
  syscall
