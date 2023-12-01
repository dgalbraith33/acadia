#pragma once

#include <glacier/container/hash_map.h>
#include <glacier/memory/shared_ptr.h>
#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <mammoth/sync/semaphore.h>
#include <victoriafalls/victoriafalls.yunq.client.h>

#include "hw/pcie.h"
#include "lib/yellowstone/yellowstone.yunq.server.h"

namespace yellowstone {

class YellowstoneServer : public YellowstoneServerBase {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<YellowstoneServer>> Create();

  glcr::Status HandleGetAhciInfo(AhciInfo&) override;
  glcr::Status HandleGetFramebufferInfo(FramebufferInfo&) override;
  glcr::Status HandleGetDenali(DenaliInfo&) override;
  glcr::Status HandleRegisterEndpoint(const RegisterEndpointRequest&) override;
  glcr::Status HandleGetEndpoint(const GetEndpointRequest&, Endpoint&) override;

  void WaitDenaliRegistered();
  void WaitVictoriaFallsRegistered();

 private:
  glcr::HashMap<glcr::String, z_cap_t> endpoint_map_;

  uint64_t device_id_ = 0;
  uint64_t lba_offset_ = 0;
  glcr::SharedPtr<VFSClient> vfs_client_;

  PciReader pci_reader_;

  mmth::Semaphore has_denali_semaphore_;
  mmth::Semaphore has_victoriafalls_semaphore_;

  YellowstoneServer(z_cap_t endpoint_cap);
};

}  // namespace yellowstone
