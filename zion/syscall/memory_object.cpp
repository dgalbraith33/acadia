#include "syscall/memory_object.h"

#include "boot/acpi.h"
#include "memory/physical_memory.h"
#include "scheduler/scheduler.h"

z_err_t MemoryObjectCreate(ZMemoryObjectCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  *req->vmmo_cap = curr_proc.AddNewCapability(
      glcr::MakeRefCounted<MemoryObject>(req->size), ZC_WRITE);
  return glcr::OK;
}

z_err_t MemoryObjectCreatePhysical(ZMemoryObjectCreatePhysicalReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t paddr = req->paddr;
  auto vmmo_ref = glcr::MakeRefCounted<FixedMemoryObject>(paddr, req->size);
  *req->vmmo_cap = curr_proc.AddNewCapability(
      StaticCastRefPtr<MemoryObject>(vmmo_ref), ZC_WRITE);
  return glcr::OK;
}

z_err_t MemoryObjectCreateContiguous(ZMemoryObjectCreateContiguousReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t paddr = phys_mem::AllocateContinuous(((req->size - 1) / 0x1000) + 1);
  auto vmmo_ref = glcr::MakeRefCounted<FixedMemoryObject>(paddr, req->size);
  *req->vmmo_cap = curr_proc.AddNewCapability(
      StaticCastRefPtr<MemoryObject>(vmmo_ref), ZC_WRITE);
  *req->paddr = paddr;
  return glcr::OK;
}

z_err_t TempPcieConfigObjectCreate(ZTempPcieConfigObjectCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t pci_base, pci_size;
  RET_ERR(GetPciExtendedConfiguration(&pci_base, &pci_size));
  auto vmmo_ref = glcr::MakeRefCounted<FixedMemoryObject>(pci_base, pci_size);
  *req->vmmo_cap = curr_proc.AddNewCapability(
      StaticCastRefPtr<MemoryObject>(vmmo_ref), ZC_WRITE);
  *req->vmmo_size = pci_size;
  return glcr::OK;
}
