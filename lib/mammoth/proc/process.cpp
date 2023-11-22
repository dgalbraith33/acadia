#include "proc/process.h"

#include <glacier/status/error.h>
#include <zcall.h>

#include "ipc/endpoint_server.h"
#include "ipc/port_client.h"
#include "ipc/port_server.h"
#include "util/debug.h"
#include "util/init.h"

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
    uint64_t page_offset = program.vaddr & 0xFFF;
    uint64_t mem_cap;
    uint64_t size = page_offset + program.memsz;
    check(ZMemoryObjectCreate(size, &mem_cap));

#if MAM_PROC_DEBUG
    dbgln("Map Local");
#endif
    uint64_t vaddr;
    check(ZAddressSpaceMap(gSelfVmasCap, 0, mem_cap, &vaddr));
    uint8_t* offset = reinterpret_cast<uint8_t*>(vaddr);
    for (uint64_t j = 0; j < size; j++) {
      offset[j] = 0;
    }

#if MAM_PROC_DEBUG
    dbgln("Copy");
#endif
    memcpy(base + program.offset, program.filesz, vaddr + page_offset);

#if MAM_PROC_DEBUG
    dbgln("Map Foreign");
#endif
    check(
        ZAddressSpaceMap(as_cap, program.vaddr - page_offset, mem_cap, &vaddr));
  }
  return header->entry;
}

}  // namespace

glcr::ErrorCode SpawnProcessFromElfRegion(uint64_t program,
                                          z_cap_t yellowstone_client) {
  uint64_t proc_cap;
  uint64_t as_cap;
  uint64_t foreign_port_id;
  uint64_t port_cap;

#if MAM_PROC_DEBUG
  dbgln("Port Create");
#endif
  ASSIGN_OR_RETURN(PortServer server, PortServer::Create());
  ASSIGN_OR_RETURN(PortClient pclient, server.CreateClient());

#if MAM_PROC_DEBUG
  dbgln("Spawn");
#endif
  RET_ERR(ZProcessSpawn(gSelfProcCap, server.cap(), &proc_cap, &as_cap,
                        &foreign_port_id));

  uint64_t entry_point = LoadElfProgram(program, as_cap);

#if MAM_PROC_DEBUG
  dbgln("Thread Create");
#endif
  uint64_t thread_cap;
  RET_ERR(ZThreadCreate(proc_cap, &thread_cap));

  RET_ERR(pclient.WriteMessage<uint64_t>(Z_INIT_SELF_PROC, proc_cap));
  RET_ERR(pclient.WriteMessage<uint64_t>(Z_INIT_SELF_VMAS, as_cap));
  RET_ERR(pclient.WriteMessage<uint64_t>(Z_INIT_ENDPOINT, yellowstone_client));

#if MAM_PROC_DEBUG
  dbgln("Thread start");
#endif
  RET_ERR(ZThreadStart(thread_cap, entry_point, foreign_port_id, 0));

  return glcr::OK;
}
