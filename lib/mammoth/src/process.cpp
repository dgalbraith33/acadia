#include "mammoth/process.h"

#include <zcall.h>
#include <zerrors.h>
#include <zinit.h>

#include "mammoth/channel.h"
#include "mammoth/debug.h"
#include "mammoth/init.h"

#define MAM_PROC_DEBUG 0

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
#if MAM_PROC_DEBUG
    dbgln("Create mem object");
#endif
    uint64_t mem_cap;
    uint64_t size = program.memsz;
    check(ZMemoryObjectCreate(size, &mem_cap));

#if MAM_PROC_DEBUG
    dbgln("Map Local");
#endif
    uint64_t vaddr;
    check(ZAddressSpaceMap(gSelfVmasCap, 0, mem_cap, &vaddr));

#if MAM_PROC_DEBUG
    dbgln("Copy");
#endif
    memcpy(base + program.offset, program.filesz, vaddr);

#if MAM_PROC_DEBUG
    dbgln("Map Foreign");
#endif
    check(ZAddressSpaceMap(as_cap, program.vaddr, mem_cap, &vaddr));
  }
  return header->entry;
}

}  // namespace

uint64_t SpawnProcessFromElfRegion(uint64_t program, Channel& local) {
  Channel foreign;
  check(CreateChannels(local, foreign));

#if MAM_PROC_DEBUG
  dbgln("Spawn");
#endif
  uint64_t proc_cap;
  uint64_t as_cap;
  uint64_t foreign_chan_id;
  check(ZProcessSpawn(gSelfProcCap, foreign.release_cap(), &proc_cap, &as_cap,
                      &foreign_chan_id));

  uint64_t entry_point = LoadElfProgram(program, as_cap);
#if MAM_PROC_DEBUG
  dbgln("Thread Create");
#endif
  uint64_t thread_cap;
  check(ZThreadCreate(proc_cap, &thread_cap));

#if MAM_PROC_DEBUG
  dbgln("Thread start");
#endif
  check(ZThreadStart(thread_cap, entry_point, foreign_chan_id, 0));

  return Z_OK;
}
