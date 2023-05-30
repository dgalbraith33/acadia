#include "loader/init_loader.h"

#include "boot/boot_info.h"
#include "debug/debug.h"
#include "loader/elf_loader.h"
#include "scheduler/process.h"
#include "scheduler/process_manager.h"

namespace {

const limine_file& GetInitProgram() {
  const limine_module_response& resp = boot::GetModules();
  dbgln("Dumping modules");
  for (uint64_t i = 0; i < resp.module_count; i++) {
    const limine_file& file = *resp.modules[i];
    dbgln("%s,%m,%x", file.path, file.address, file.size);
    // TODO eventually compare this file path.
    return file;
  }
  panic("No init program found");
}

}  // namespace

void LoadInitProgram() {
  const limine_file& init_prog = GetInitProgram();

  gProcMan->InsertProcess(
      new Process(reinterpret_cast<uint64_t>(init_prog.address)));
}
