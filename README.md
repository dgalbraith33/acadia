# AcadiaOS

AcadiaOS is a hobby operating system I use to explore and learn about OS development.

It is built on top of a small capability-based microkernel that provides
memory management, process scheduling, and hardware access.

## Directory Structure

The subdirectories of the project are as follows:
- **zion**: Contains kernel code as well as a couple headers that user space uses to interface with 
the kernel.
- **lib**: Library code used by the kernel and user space.
- **sys**: System services.
- **sysroot**: Files that are copied to the root filesystem.
- **toolchain** and **scripts**: Contain tooling for the cross-compile environment.
- **yunq**: Contains the El Yunque IDL used for IPC between processes in userspace.

Where available, further documentation about each of these components can be found in the README fi
les in each respective subdirectory.

## Building AcadiaOS

Run `./scripts/build_toolchain.sh` to build a gcc cross-compile toolchain for the project. This
step may take a long time (~5 mins on my new laptop, ~45 mins on my 2013 laptop).

Run `./init-dbg.sh` to initialize the CMake project.

Run `./scripts/qemu.sh` to build and run AcadiaOS in a VM. I usually run this from the `builddbg`
directory so it is easier to run `ninja clean` when necessary (for instance when updating yunq
files).

### System Dependencies

On arch linux I require the following packages to build this project:

`base-devel limine dosfstools parted cmake ninja qemu-desktop qemu-system-x86`

## Boot Process

AcadiaOS uses the limine bootloader to enter into a 64-bit kernel in the negative 2GB address space 
(mcmodel kernel).

The Zion kernel then sets up interrupts, scheduling, and memory management before passing off 
control to the Yellowstone process.

The kernel passes capabilities to the Denali and VictoriaFalls binaries to the Yellowstone process 
to allow it to manage the rest of the boot process.

The Yellowstone, Denali, and VictoriaFalls processes are all passed at boot using limine modules, 
however any subsequent processes are loaded off disk.
