#include "loader/init_loader.h"

#include <glacier/memory/ref_ptr.h>
#include <glacier/string/string.h>

#include "boot/boot_info.h"
#include "debug/debug.h"
#include "include/zcall.h"
#include "memory/paging_util.h"
#include "object/process.h"
#include "object/thread.h"
#include "scheduler/process_manager.h"
#include "scheduler/scheduler.h"

#define K_INIT_DEBUG 0

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

uint64_t LoadElfProgram(Process& dest_proc, uint64_t base, uint64_t offset) {
  Elf64Header* header = reinterpret_cast<Elf64Header*>(base);
#if K_INIT_DEBUG
  dbgln("phoff: %u phnum: %u", header->phoff, header->phnum);
#endif
  Elf64ProgramHeader* programs =
      reinterpret_cast<Elf64ProgramHeader*>(base + header->phoff);
  for (uint64_t i = 0; i < header->phnum; i++) {
    Elf64ProgramHeader& program = programs[i];
#if K_INIT_DEBUG
    dbgln(
        "prog: type: %u, flags: %u, offset: %u\n  vaddr: %m, paddr: %m\n  "
        "filesz: %x, memsz: %x, align: %x",
        program.type, program.flags, program.offset, program.vaddr,
        program.paddr, program.filesz, program.memsz, program.align);
#endif
    auto mem_obj = glcr::MakeRefCounted<MemoryObject>(program.memsz);
    mem_obj->CopyBytesToObject(base + program.offset, program.filesz);
    dest_proc.vmas()->MapInMemoryObject(program.vaddr, mem_obj);
  }
  return header->entry;
}

bool streq(const char* a, const char* b) {
  while (true) {
    if (*a == '\0' && *b == '\0') return true;
    if (*a == '\0' || *b == '\0') {
      return false;
    }
    if (*a != *b) {
      return false;
    }
    a++;
    b++;
  }
}

void DumpModules() {
#if K_INIT_DEBUG
  const limine_module_response& resp = boot::GetModules();
  dbgln("[boot] Dumping bootloader modules.");
  for (uint64_t i = 0; i < resp.module_count; i++) {
    const limine_file& file = *resp.modules[i];
    dbgln("    %s,%m,%x", file.path, file.address, file.size);
  }
#endif
}

const limine_file& GetInitProgram(glcr::String path) {
  const limine_module_response& resp = boot::GetModules();
  for (uint64_t i = 0; i < resp.module_count; i++) {
    const limine_file& file = *resp.modules[i];
    if (path == file.path) {
      return file;
    }
  }
  panic("Program not found: %s", path);
  UNREACHABLE
}

void WriteInitProgram(glcr::RefPtr<Port> port, glcr::String name, uint64_t id) {
  const limine_file& prog = GetInitProgram(name);
  glcr::RefPtr<MemoryObject> prog_vmmo =
      glcr::MakeRefCounted<MemoryObject>(prog.size);
  prog_vmmo->CopyBytesToObject(reinterpret_cast<uint64_t>(prog.address),
                               prog.size);
  port->WriteKernel(id,
                    MakeRefCounted<Capability>(prog_vmmo, ZC_READ | ZC_WRITE));
}

}  // namespace

void LoadInitProgram() {
  DumpModules();

  // Create process.
  glcr::RefPtr<Process> proc = Process::Create();
  gProcMan->InsertProcess(proc);

  // Write init data.
  auto port = glcr::MakeRefCounted<Port>();
  uint64_t port_cap = proc->AddNewCapability(port, ZC_READ | ZC_WRITE);
  port->WriteKernel(Z_INIT_SELF_PROC,
                    MakeRefCounted<Capability>(
                        proc, ZC_PROC_SPAWN_PROC | ZC_PROC_SPAWN_THREAD));
  port->WriteKernel(Z_INIT_SELF_VMAS,
                    MakeRefCounted<Capability>(proc->vmas(), ZC_WRITE));
  WriteInitProgram(port, "/sys/denali", Z_BOOT_DENALI_VMMO);
  WriteInitProgram(port, "/sys/victoriafalls", Z_BOOT_VICTORIA_FALLS_VMMO);

  // Start process.
  const limine_file& init_prog = GetInitProgram("/sys/yellowstone");
  uint64_t entry = LoadElfProgram(
      *proc, reinterpret_cast<uint64_t>(init_prog.address), init_prog.size);
  proc->CreateThread()->Start(entry, port_cap, 0);
}
