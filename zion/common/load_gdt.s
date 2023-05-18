.global ReloadSegments
ReloadSegments:
  push $0x08
  lea ._reload_cs, %rax
  push %rax
  retfq

._reload_cs:
  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  mov %ax, %ss
  ret
