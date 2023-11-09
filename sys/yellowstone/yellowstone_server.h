#pragma once

#include <glacier/memory/shared_ptr.h>
#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <mammoth/endpoint_server.h>
#include <mammoth/mutex.h>
#include <mammoth/port_server.h>
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

  glcr::ErrorCode WaitDenaliRegistered();
  glcr::ErrorCode WaitVictoriaFallsRegistered();

  glcr::SharedPtr<VFSClient> GetVFSClient();

 private:
  // TODO: Store these in a data structure.
  z_cap_t denali_cap_ = 0;
  uint64_t device_id_ = 0;
  uint64_t lba_offset_ = 0;
  z_cap_t victoria_falls_cap_ = 0;
  glcr::SharedPtr<VFSClient> vfs_client_;

  PciReader pci_reader_;

  Mutex has_denali_mutex_;
  Mutex has_victoriafalls_mutex_;

  YellowstoneServer(z_cap_t endpoint_cap, Mutex&& denali_mutex,
                    Mutex&& victoriafalls_mutex);
};
