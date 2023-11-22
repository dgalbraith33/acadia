# Zion TODO

## Memory Management

### Physical Memory

- Reserve lower physical memory for hardware accesses as necessary.
- Return errors rather than panicking on allocation failure

### Virtual Memory

- Allow different permissions and caching behaviors on mapped memory.
- Add diagnostics to inspect process memory usage.
- Properly free user stacks on thread exit.

### Allocation

- Use a buddy allocator with a real free when an object is too large for
  the slab allocator.
- Free kernel stacks on thread/process exit.

## Process Management

- Clean up process state and capabilities when it exits.

## Processor

- Enable all instruction types before passing to user space.

## Faults and Interrupts

- Exit process rather than panicking on unhandled Page Fault (and others).
- Add additional fault handlers to avoid GP faults.
- Improve process for registering interrupt handlers.

## Capabilities

- Add syscalls for inspecting capabilities.
- Add syscalls for restricting capabilities' permissions.
- Randomize/obfuscate capability numbers passed to user space.
- Remove ReplyPort capabilities once the response is sent.

## Scheduling

- Add different scheduling priorities.
- Add thread sleep capability.

## Large Projects

- Add multiprocessing support.
- Add layer on top of limine to enable multiple bootloaders.
- Add a large amount of debugging info for kernel panics.
