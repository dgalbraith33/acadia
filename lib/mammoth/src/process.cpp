#include "include/mammoth/process.h"

#include <zcall.h>
#include <zerrors.h>

#include "include/mammoth/debug.h"

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

void memcpy(uint64_t base, uint64_t len, uint64_t dest) {
  uint8_t* srcptr = reinterpret_cast<uint8_t*>(base);
  uint8_t* destptr = reinterpret_cast<uint8_t*>(dest);
  for (uint64_t i = 0; i < len; i++) {
    destptr[i] = srcptr[i];
  }
}

uint64_t LoadElfProgram(uint64_t base, uint64_t as_cap) {
  Elf64Header* header = reinterpret_cast<Elf64Header*>(base);
  Elf64ProgramHeader* programs =
      reinterpret_cast<Elf64ProgramHeader*>(base + header->phoff);
  for (uint64_t i = 0; i < header->phnum; i++) {
    Elf64ProgramHeader& program = programs[i];
    dbgln("Create mem object");
    uint64_t mem_cap;
    uint64_t size = program.filesz;
    check(ZMemoryObjectCreate(size, &mem_cap));

    dbgln("Map Local");
    uint64_t vaddr;
    check(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, mem_cap, &vaddr));

    dbgln("Copy");
    memcpy(base + program.offset, program.filesz, vaddr);

    dbgln("Map Foreign");
    check(ZAddressSpaceMap(as_cap, program.vaddr, mem_cap, &vaddr));
  }
  return header->entry;
}

}  // namespace

uint64_t SpawnProcessFromElfRegion(uint64_t program) {
  dbgln("Channel Create");
  uint64_t local_chan;
  uint64_t foreign_chan;
  check(ZChannelCreate(&local_chan, &foreign_chan));

  dbgln("Spawn");
  uint64_t proc_cap;
  uint64_t as_cap;
  check(ZProcessSpawn(Z_INIT_PROC_SELF, foreign_chan, &proc_cap, &as_cap,
                      &foreign_chan));

  uint64_t entry_point = LoadElfProgram(program, as_cap);
  dbgln("Thread Create");
  uint64_t thread_cap;
  check(ZThreadCreate(proc_cap, &thread_cap));

  dbgln("Thread start");
  check(ZThreadStart(thread_cap, entry_point, foreign_chan, 0));

  const uint8_t* msg = reinterpret_cast<const uint8_t*>("Hello!");
  check(ZChannelSend(local_chan, 0, 7, msg, 0, 0));

  return Z_OK;
}
