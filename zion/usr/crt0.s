.text

.global _start
_start:
  call main
  call _exit

_exit:
  mov $1, %rdi
  syscall
