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

glcr::ErrorCode YellowstoneStub::Register(glcr::String name,
                                          const EndpointClient& client) {
  if (register_port_.empty()) {
    YellowstoneGetReq req{
        .type = kYellowstoneGetRegistration,
    };
    ASSIGN_OR_RETURN(
        auto resp_cap,
        (yellowstone_stub_->CallEndpointGetCap<
            YellowstoneGetReq, YellowstoneGetRegistrationResp>(req)));
    register_port_ = PortClient::AdoptPort(resp_cap.second());
  }

  return register_port_.WriteString(name, client.GetCap());
}
