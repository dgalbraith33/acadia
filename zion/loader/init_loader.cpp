#include "loader/init_loader.h"

#include "boot/boot_info.h"
#include "debug/debug.h"
#include "loader/elf_loader.h"
#include "memory/paging_util.h"
#include "scheduler/process.h"
#include "scheduler/process_manager.h"

namespace {

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

const limine_file& GetInitProgram(const char* path) {
  const limine_module_response& resp = boot::GetModules();
  dbgln("Dumping modules");
  for (uint64_t i = 0; i < resp.module_count; i++) {
    const limine_file& file = *resp.modules[i];
    dbgln("%s,%m,%x", file.path, file.address, file.size);
    if (streq(file.path, path)) return file;
  }
  panic("Program not found: %s", path);
}

}  // namespace

void LoadInitProgram() {
  const limine_file& init_prog = GetInitProgram("/sys/test");
  const limine_file& prog2 = GetInitProgram("/sys/test2");

  SharedPtr<Process> proc = MakeShared<Process>();
  gProcMan->InsertProcess(proc);

  uint64_t entry = LoadElfProgram(
      *proc, reinterpret_cast<uint64_t>(init_prog.address), init_prog.size);

  CopyIntoNonResidentProcess(reinterpret_cast<uint64_t>(prog2.address),
                             prog2.size, *proc,
                             proc->vmm().GetNextMemMapAddr(prog2.size));

  proc->CreateThread(entry);
}
