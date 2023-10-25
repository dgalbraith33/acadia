# Zion Microkernel

The Zion Microkernel is a capability based kernel. The types of objects that
processes can hold a capability to exist in the `object/` directory. The system calls
used to interact with them are enumerated in `include/zcall.h` and are implemented
in the `syscalls/` directory.

The system uses a barebones ELF loader (`loader/init_loader.h`) to jump to userspace
but from there userspace processes are responsible for loading child processes into
memory and executing them.

## Open Work

The [TODO file](TODO.md) enumerates current areas for improvement.
