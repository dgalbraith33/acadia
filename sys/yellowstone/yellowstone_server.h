#pragma once

#include <glacier/container/hash_map.h>
#include <glacier/memory/shared_ptr.h>
#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <mammoth/semaphore.h>
#include <mammoth/thread.h>
#include <victoriafalls/victoriafalls.yunq.client.h>

#include "hw/pcie.h"
#include "lib/yellowstone/yellowstone.yunq.server.h"

class YellowstoneServer : public YellowstoneServerBase {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<YellowstoneServer>> Create();

  glcr::ErrorCode HandleGetAhciInfo(const Empty&, AhciInfo&) override;
  glcr::ErrorCode HandleGetFramebufferInfo(const Empty&,
                                           FramebufferInfo&) override;
  glcr::ErrorCode HandleGetDenali(const Empty&, DenaliInfo&) override;
  glcr::ErrorCode HandleRegisterEndpoint(const RegisterEndpointRequest&,
                                         Empty&) override;
  glcr::ErrorCode HandleGetEndpoint(const GetEndpointRequest&,
                                    Endpoint&) override;

  void WaitDenaliRegistered();
  void WaitVictoriaFallsRegistered();

  glcr::SharedPtr<VFSClient> GetVFSClient();

 private:
  glcr::HashMap<glcr::String, z_cap_t> endpoint_map_;

  uint64_t device_id_ = 0;
  uint64_t lba_offset_ = 0;
  glcr::SharedPtr<VFSClient> vfs_client_;

  PciReader pci_reader_;

  Semaphore has_denali_semaphore_;
  Semaphore has_victoriafalls_semaphore_;

  YellowstoneServer(z_cap_t endpoint_cap);
};
