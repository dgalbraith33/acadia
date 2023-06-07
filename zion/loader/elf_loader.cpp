#include "loader/elf_loader.h"

#include "debug/debug.h"
#include "memory/paging_util.h"

namespace {

typedef struct {
  char ident[16];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  uint64_t entry;
  uint64_t phoff;
  uint64_t shoff;
  uint32_t flags;
  uint16_t ehsize;
  uint16_t phentsize;
  uint16_t phnum;
  uint16_t shentsize;
  uint16_t shnum;
  uint16_t shstrndx;
} Elf64Header;

typedef struct {
  uint32_t name;
  uint32_t type;
  uint64_t flags;
  uint64_t addr;
  uint64_t offset;
  uint64_t size;
  uint32_t link;
  uint32_t info;
  uint64_t addralign;
  uint64_t entsize;
} Elf64SectionHeader;

typedef struct {
  uint32_t type;
  uint32_t flags;
  uint64_t offset;
  uint64_t vaddr;
  uint64_t paddr;
  uint64_t filesz;
  uint64_t memsz;
  uint64_t align;
} Elf64ProgramHeader;

}  // namespace

uint64_t LoadElfProgram(Process& dest_proc, uint64_t base, uint64_t offset) {
  Elf64Header* header = reinterpret_cast<Elf64Header*>(base);
  dbgln("phoff: %u phnum: %u", header->phoff, header->phnum);
  Elf64ProgramHeader* programs =
      reinterpret_cast<Elf64ProgramHeader*>(base + header->phoff);
  for (uint64_t i = 0; i < header->phnum; i++) {
    Elf64ProgramHeader& program = programs[i];
    dbgln(
        "prog: type: %u, flags: %u, offset: %u\n  vaddr: %m, paddr: %m\n  "
        "filesz: %x, memsz: %x, align: %x",
        program.type, program.flags, program.offset, program.vaddr,
        program.paddr, program.filesz, program.memsz, program.align);
    auto mem_obj = MakeRefCounted<MemoryObject>(program.filesz);
    mem_obj->CopyBytesToObject(base + program.offset, program.filesz);
    dest_proc.vmm().MapInMemoryObject(program.vaddr, mem_obj);
  }
  return header->entry;
}
