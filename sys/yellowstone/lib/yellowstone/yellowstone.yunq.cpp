// Generated file -- DO NOT MODIFY.
#include "yellowstone.yunq.h"

#include <yunq/message_view.h>
#include <yunq/serialize.h>


namespace yellowstone {

namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status RegisterEndpointRequest::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse endpoint_capability.
  ASSIGN_OR_RETURN(endpoint_capability_, message.ReadCapability(1));
  return glcr::Status::Ok();
}

glcr::Status RegisterEndpointRequest::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse endpoint_capability.
  ASSIGN_OR_RETURN(endpoint_capability_, message.ReadCapability(1, caps));
  return glcr::Status::Ok();
}

glcr::Status RegisterEndpointRequest::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse endpoint_name.
  ASSIGN_OR_RETURN(endpoint_name_, message.ReadField<glcr::String>(0));
  // Parse endpoint_capability.

  return glcr::Status::Ok();
}

uint64_t RegisterEndpointRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 2);
  return SerializeInternal(serializer);
}

uint64_t RegisterEndpointRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 2, caps);
  return SerializeInternal(serializer);
}
  
uint64_t RegisterEndpointRequest::SerializeInternal(yunq::Serializer& serializer) const {
  // Write endpoint_name.
  serializer.WriteField<glcr::String>(0, endpoint_name_);
  // Write endpoint_capability.
  serializer.WriteCapability(1, endpoint_capability_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status GetEndpointRequest::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status GetEndpointRequest::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status GetEndpointRequest::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse endpoint_name.
  ASSIGN_OR_RETURN(endpoint_name_, message.ReadField<glcr::String>(0));

  return glcr::Status::Ok();
}

uint64_t GetEndpointRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t GetEndpointRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t GetEndpointRequest::SerializeInternal(yunq::Serializer& serializer) const {
  // Write endpoint_name.
  serializer.WriteField<glcr::String>(0, endpoint_name_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status Endpoint::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse endpoint.
  ASSIGN_OR_RETURN(endpoint_, message.ReadCapability(0));
  return glcr::Status::Ok();
}

glcr::Status Endpoint::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse endpoint.
  ASSIGN_OR_RETURN(endpoint_, message.ReadCapability(0, caps));
  return glcr::Status::Ok();
}

glcr::Status Endpoint::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse endpoint.

  return glcr::Status::Ok();
}

uint64_t Endpoint::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t Endpoint::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t Endpoint::SerializeInternal(yunq::Serializer& serializer) const {
  // Write endpoint.
  serializer.WriteCapability(0, endpoint_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status AhciInfo::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse ahci_region.
  ASSIGN_OR_RETURN(ahci_region_, message.ReadCapability(0));
  return glcr::Status::Ok();
}

glcr::Status AhciInfo::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse ahci_region.
  ASSIGN_OR_RETURN(ahci_region_, message.ReadCapability(0, caps));
  return glcr::Status::Ok();
}

glcr::Status AhciInfo::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse ahci_region.
  // Parse region_length.
  ASSIGN_OR_RETURN(region_length_, message.ReadField<uint64_t>(1));

  return glcr::Status::Ok();
}

uint64_t AhciInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 2);
  return SerializeInternal(serializer);
}

uint64_t AhciInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 2, caps);
  return SerializeInternal(serializer);
}
  
uint64_t AhciInfo::SerializeInternal(yunq::Serializer& serializer) const {
  // Write ahci_region.
  serializer.WriteCapability(0, ahci_region_);
  // Write region_length.
  serializer.WriteField<uint64_t>(1, region_length_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status XhciInfo::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse xhci_region.
  ASSIGN_OR_RETURN(xhci_region_, message.ReadCapability(0));
  return glcr::Status::Ok();
}

glcr::Status XhciInfo::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse xhci_region.
  ASSIGN_OR_RETURN(xhci_region_, message.ReadCapability(0, caps));
  return glcr::Status::Ok();
}

glcr::Status XhciInfo::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse xhci_region.
  // Parse region_length.
  ASSIGN_OR_RETURN(region_length_, message.ReadField<uint64_t>(1));

  return glcr::Status::Ok();
}

uint64_t XhciInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 2);
  return SerializeInternal(serializer);
}

uint64_t XhciInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 2, caps);
  return SerializeInternal(serializer);
}
  
uint64_t XhciInfo::SerializeInternal(yunq::Serializer& serializer) const {
  // Write xhci_region.
  serializer.WriteCapability(0, xhci_region_);
  // Write region_length.
  serializer.WriteField<uint64_t>(1, region_length_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status FramebufferInfo::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status FramebufferInfo::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status FramebufferInfo::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse address_phys.
  ASSIGN_OR_RETURN(address_phys_, message.ReadField<uint64_t>(0));
  // Parse width.
  ASSIGN_OR_RETURN(width_, message.ReadField<uint64_t>(1));
  // Parse height.
  ASSIGN_OR_RETURN(height_, message.ReadField<uint64_t>(2));
  // Parse pitch.
  ASSIGN_OR_RETURN(pitch_, message.ReadField<uint64_t>(3));
  // Parse bpp.
  ASSIGN_OR_RETURN(bpp_, message.ReadField<uint64_t>(4));
  // Parse memory_model.
  ASSIGN_OR_RETURN(memory_model_, message.ReadField<uint64_t>(5));
  // Parse red_mask_size.
  ASSIGN_OR_RETURN(red_mask_size_, message.ReadField<uint64_t>(6));
  // Parse red_mask_shift.
  ASSIGN_OR_RETURN(red_mask_shift_, message.ReadField<uint64_t>(7));
  // Parse green_mask_size.
  ASSIGN_OR_RETURN(green_mask_size_, message.ReadField<uint64_t>(8));
  // Parse green_mask_shift.
  ASSIGN_OR_RETURN(green_mask_shift_, message.ReadField<uint64_t>(9));
  // Parse blue_mask_size.
  ASSIGN_OR_RETURN(blue_mask_size_, message.ReadField<uint64_t>(10));
  // Parse blue_mask_shift.
  ASSIGN_OR_RETURN(blue_mask_shift_, message.ReadField<uint64_t>(11));

  return glcr::Status::Ok();
}

uint64_t FramebufferInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 12);
  return SerializeInternal(serializer);
}

uint64_t FramebufferInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 12, caps);
  return SerializeInternal(serializer);
}
  
uint64_t FramebufferInfo::SerializeInternal(yunq::Serializer& serializer) const {
  // Write address_phys.
  serializer.WriteField<uint64_t>(0, address_phys_);
  // Write width.
  serializer.WriteField<uint64_t>(1, width_);
  // Write height.
  serializer.WriteField<uint64_t>(2, height_);
  // Write pitch.
  serializer.WriteField<uint64_t>(3, pitch_);
  // Write bpp.
  serializer.WriteField<uint64_t>(4, bpp_);
  // Write memory_model.
  serializer.WriteField<uint64_t>(5, memory_model_);
  // Write red_mask_size.
  serializer.WriteField<uint64_t>(6, red_mask_size_);
  // Write red_mask_shift.
  serializer.WriteField<uint64_t>(7, red_mask_shift_);
  // Write green_mask_size.
  serializer.WriteField<uint64_t>(8, green_mask_size_);
  // Write green_mask_shift.
  serializer.WriteField<uint64_t>(9, green_mask_shift_);
  // Write blue_mask_size.
  serializer.WriteField<uint64_t>(10, blue_mask_size_);
  // Write blue_mask_shift.
  serializer.WriteField<uint64_t>(11, blue_mask_shift_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status DenaliInfo::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse denali_endpoint.
  ASSIGN_OR_RETURN(denali_endpoint_, message.ReadCapability(0));
  return glcr::Status::Ok();
}

glcr::Status DenaliInfo::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse denali_endpoint.
  ASSIGN_OR_RETURN(denali_endpoint_, message.ReadCapability(0, caps));
  return glcr::Status::Ok();
}

glcr::Status DenaliInfo::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse denali_endpoint.
  // Parse device_id.
  ASSIGN_OR_RETURN(device_id_, message.ReadField<uint64_t>(1));
  // Parse lba_offset.
  ASSIGN_OR_RETURN(lba_offset_, message.ReadField<uint64_t>(2));

  return glcr::Status::Ok();
}

uint64_t DenaliInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 3);
  return SerializeInternal(serializer);
}

uint64_t DenaliInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 3, caps);
  return SerializeInternal(serializer);
}
  
uint64_t DenaliInfo::SerializeInternal(yunq::Serializer& serializer) const {
  // Write denali_endpoint.
  serializer.WriteCapability(0, denali_endpoint_);
  // Write device_id.
  serializer.WriteField<uint64_t>(1, device_id_);
  // Write lba_offset.
  serializer.WriteField<uint64_t>(2, lba_offset_);

  serializer.WriteHeader();

  return serializer.size();
}


}  // namepace yellowstone
