#include "include/yellowstone_stub.h"

#include "include/yellowstone.h"

namespace {

const uint64_t kPciSize = 0x1000;

}  // namespace

YellowstoneStub::YellowstoneStub(z_cap_t yellowstone_cap)
    : yellowstone_stub_(EndpointClient::AdoptEndpoint(yellowstone_cap)) {}

glcr::ErrorOr<MappedMemoryRegion> YellowstoneStub::GetAhciConfig() {
  YellowstoneGetReq req{
      .type = kYellowstoneGetAhci,
  };
  ASSIGN_OR_RETURN(
      YellowstoneGetAhciResp resp,
      (yellowstone_stub_
           ->CallEndpoint<YellowstoneGetReq, YellowstoneGetAhciResp>(req)));
  return MappedMemoryRegion::DirectPhysical(resp.ahci_phys_offset, kPciSize);
}
