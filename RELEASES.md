# Releases

## AcadiaOS 0.1.1 (WIP)

### Denali

- AHCI Driver can use more than one command slot.
- Resets AHCI Controller on start.
- Uses IDENTIFY DEVICE to get sector size.

### Glacier

- Unit Testing setup for Host Machine
- Unit Tests for: Vector
- Added Iterators for: Vector, Array, ArrayView
- HashMap Move Semantics

### Yunq

- Moved message parsing/serialization to shared library.

## AcadiaOS 0.1.0 (2023-12-08)

This marks the first release of AcadiaOS! There is very little user functionality currently but a
lot of foundational work has been done.

### Zion Kernel

Capability-based microkernel.

Boot Process:
- Initializes GDT and IDT.
- Sets up a separate stack for page faults and general protection faults.
- Enables SSE.
- Minimally initializes the APIC in order to use its timer as well as map the PS/2 Keyboard and PCI
interrupts.
- Enables Syscalls using the sysenter instruction.
- Enables the scheduler.
- Loads the Yellowstone init program and yields to it.

Features:
- Physical memory manager can dole out single pages as well as continuous chunks. Freed pages and
chunks can be reused.
- Kernel memory allocation done primarily using a slab allocator for sizes up to 128 bytes. (Larger
allocations are done by simply incrementing a pointer and are never truly freed. However currently 
none occur.).
- Round robin scheduler with a single priority level.

Interface:
- Capability-based interface.
- Process Management: ProcessInit, ProcessExit.
- Thread Management: ThreadCreate, ThreadStart, ThreadExit.
- Memory Management: MemoryObjectCreate, MemoryObjectCreateContiguous, MemoryObjectCreatePhysical,
MemoryObjectDuplicate, MemoryObjectInspect, AddressSpaceMap, AddressSpaceUnmap.
- IPC with Channels, Ports, and Endpoints.
- Channel: ChannelCreate, ChannelSend, ChannelRecv.
- Ports: PortCreate, PortSend, PortRecv, PortPoll.
- Endpoints: EndpointCreate, EndpointSend, EndpointRecv, ReplyPortSend, ReplyPortRecv.
- Capability Management: CapDuplicate, CapRelease.
- Irq Management: IrqRegister.
- Synchronization: ProcessWait, ThreadWait, MutexCreate, MutexLock, MutexRelease, SemaphoreCreate,
SemaphoreWait, SemaphoreSignal.
- Debug Output: Debug.

### Yellowstone

Initialization process that starts up the other necessary boot processes then maintains a registry
of the available IPC Servers running on the system.

- Spawns the AHCI Client "Denali".
- Once it is online, reads the GPT and spawns the VFS service "VictoriaFallS".
- Once it is online, reads "init.txt" and spawns the processes listed in the file from the /bin dir.


### Denali

Barebones AHCI Driver.

- Reads the SATA PCI region the HBAs associated with it.
- Starts the Denali service to allow clients to read directly from disk. 
- Currently only one command port is used so reads are done in serial.

### VictoriaFallS

Barebones VFS Server. Currently passes all calls to it directly to the ext2 filesystem at the root
of the VFS.

- Allows reading of files and directories.
- Does not cache results or inodes so all calls go directly to the AHCI Server.


### Teton

Very basic terminal and shell.

- Supports ls, pwd, cd, and exec commands.
- All commands must use absolute paths.
- Starts a child process when using exec and waits for it to complete.

### Yunq

IDL for IPC on AcadiaOS with cpp codegen for messages, clients, and servers.

- Supports serializing capabilities for IPC.
- Supports repeated integer fields.
- Supports string files.
- Allows namespacing cpp definitions.

