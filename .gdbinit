source ~/.gdbinit-gef.py
gef-remote --qemu-user --qemu-binary=builddbg/zion/zion localhost 1234
file builddbg/zion/zion
break zion
