.text

.global _start
_start:
  call main
  call _exit

_exit:
  // EXIT syscall.
  mov $1, %rdi
  // Return code as a param.
  mov %rax, %rsi
  syscall
