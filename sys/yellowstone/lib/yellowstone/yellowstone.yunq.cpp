// Generated file -- DO NOT MODIFY.
#include "yellowstone.yunq.h"

#include <yunq/serialize.h>


namespace yellowstone {

namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status RegisterEndpointRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse endpoint_capability.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_endpoint_capability(0);
  return glcr::Status::Ok();
}

glcr::Status RegisterEndpointRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse endpoint_capability.
  uint64_t endpoint_capability_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 1));

  set_endpoint_capability(caps.At(endpoint_capability_ptr));
  return glcr::Status::Ok();
}

glcr::Status RegisterEndpointRequest::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes, offset));
  // Parse endpoint_name.
  auto endpoint_name_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_endpoint_name(bytes.StringAt(offset + endpoint_name_pointer.offset, endpoint_name_pointer.length));
  // Parse endpoint_capability.
  // Skip Cap.

  return glcr::Status::Ok();
}

uint64_t RegisterEndpointRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 2;
  const uint32_t core_size = next_extension;
  // Write endpoint_name.
  ExtPointer endpoint_name_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)endpoint_name().length(),
  };

  bytes.WriteStringAt(offset + next_extension, endpoint_name());
  next_extension += endpoint_name_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), endpoint_name_ptr);
  // Write endpoint_capability.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), 0);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t RegisterEndpointRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 2;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write endpoint_name.
  ExtPointer endpoint_name_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)endpoint_name().length(),
  };

  bytes.WriteStringAt(offset + next_extension, endpoint_name());
  next_extension += endpoint_name_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), endpoint_name_ptr);
  // Write endpoint_capability.
  caps.WriteAt(next_cap, endpoint_capability());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), next_cap++);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status GetEndpointRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status GetEndpointRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status GetEndpointRequest::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes, offset));
  // Parse endpoint_name.
  auto endpoint_name_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_endpoint_name(bytes.StringAt(offset + endpoint_name_pointer.offset, endpoint_name_pointer.length));

  return glcr::Status::Ok();
}

uint64_t GetEndpointRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  // Write endpoint_name.
  ExtPointer endpoint_name_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)endpoint_name().length(),
  };

  bytes.WriteStringAt(offset + next_extension, endpoint_name());
  next_extension += endpoint_name_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), endpoint_name_ptr);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t GetEndpointRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write endpoint_name.
  ExtPointer endpoint_name_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)endpoint_name().length(),
  };

  bytes.WriteStringAt(offset + next_extension, endpoint_name());
  next_extension += endpoint_name_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), endpoint_name_ptr);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status Endpoint::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse endpoint.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_endpoint(0);
  return glcr::Status::Ok();
}

glcr::Status Endpoint::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse endpoint.
  uint64_t endpoint_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 0));

  set_endpoint(caps.At(endpoint_ptr));
  return glcr::Status::Ok();
}

glcr::Status Endpoint::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes, offset));
  // Parse endpoint.
  // Skip Cap.

  return glcr::Status::Ok();
}

uint64_t Endpoint::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  // Write endpoint.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), 0);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t Endpoint::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write endpoint.
  caps.WriteAt(next_cap, endpoint());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), next_cap++);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status AhciInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse ahci_region.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_ahci_region(0);
  return glcr::Status::Ok();
}

glcr::Status AhciInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse ahci_region.
  uint64_t ahci_region_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 0));

  set_ahci_region(caps.At(ahci_region_ptr));
  return glcr::Status::Ok();
}

glcr::Status AhciInfo::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes, offset));
  // Parse ahci_region.
  // Skip Cap.
  // Parse region_length.
  set_region_length(bytes.At<uint64_t>(offset + header_size + (8 * 1)));

  return glcr::Status::Ok();
}

uint64_t AhciInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 2;
  const uint32_t core_size = next_extension;
  // Write ahci_region.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), 0);
  // Write region_length.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), region_length());

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t AhciInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 2;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write ahci_region.
  caps.WriteAt(next_cap, ahci_region());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), next_cap++);
  // Write region_length.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), region_length());

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status FramebufferInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status FramebufferInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status FramebufferInfo::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes, offset));
  // Parse address_phys.
  set_address_phys(bytes.At<uint64_t>(offset + header_size + (8 * 0)));
  // Parse width.
  set_width(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse height.
  set_height(bytes.At<uint64_t>(offset + header_size + (8 * 2)));
  // Parse pitch.
  set_pitch(bytes.At<uint64_t>(offset + header_size + (8 * 3)));
  // Parse bpp.
  set_bpp(bytes.At<uint64_t>(offset + header_size + (8 * 4)));
  // Parse memory_model.
  set_memory_model(bytes.At<uint64_t>(offset + header_size + (8 * 5)));
  // Parse red_mask_size.
  set_red_mask_size(bytes.At<uint64_t>(offset + header_size + (8 * 6)));
  // Parse red_mask_shift.
  set_red_mask_shift(bytes.At<uint64_t>(offset + header_size + (8 * 7)));
  // Parse green_mask_size.
  set_green_mask_size(bytes.At<uint64_t>(offset + header_size + (8 * 8)));
  // Parse green_mask_shift.
  set_green_mask_shift(bytes.At<uint64_t>(offset + header_size + (8 * 9)));
  // Parse blue_mask_size.
  set_blue_mask_size(bytes.At<uint64_t>(offset + header_size + (8 * 10)));
  // Parse blue_mask_shift.
  set_blue_mask_shift(bytes.At<uint64_t>(offset + header_size + (8 * 11)));

  return glcr::Status::Ok();
}

uint64_t FramebufferInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 12;
  const uint32_t core_size = next_extension;
  // Write address_phys.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), address_phys());
  // Write width.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), width());
  // Write height.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), height());
  // Write pitch.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 3), pitch());
  // Write bpp.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 4), bpp());
  // Write memory_model.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 5), memory_model());
  // Write red_mask_size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 6), red_mask_size());
  // Write red_mask_shift.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 7), red_mask_shift());
  // Write green_mask_size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 8), green_mask_size());
  // Write green_mask_shift.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 9), green_mask_shift());
  // Write blue_mask_size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 10), blue_mask_size());
  // Write blue_mask_shift.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 11), blue_mask_shift());

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t FramebufferInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 12;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write address_phys.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), address_phys());
  // Write width.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), width());
  // Write height.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), height());
  // Write pitch.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 3), pitch());
  // Write bpp.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 4), bpp());
  // Write memory_model.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 5), memory_model());
  // Write red_mask_size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 6), red_mask_size());
  // Write red_mask_shift.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 7), red_mask_shift());
  // Write green_mask_size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 8), green_mask_size());
  // Write green_mask_shift.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 9), green_mask_shift());
  // Write blue_mask_size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 10), blue_mask_size());
  // Write blue_mask_shift.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 11), blue_mask_shift());

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status DenaliInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse denali_endpoint.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_denali_endpoint(0);
  return glcr::Status::Ok();
}

glcr::Status DenaliInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse denali_endpoint.
  uint64_t denali_endpoint_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 0));

  set_denali_endpoint(caps.At(denali_endpoint_ptr));
  return glcr::Status::Ok();
}

glcr::Status DenaliInfo::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes, offset));
  // Parse denali_endpoint.
  // Skip Cap.
  // Parse device_id.
  set_device_id(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse lba_offset.
  set_lba_offset(bytes.At<uint64_t>(offset + header_size + (8 * 2)));

  return glcr::Status::Ok();
}

uint64_t DenaliInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  // Write denali_endpoint.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), 0);
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), device_id());
  // Write lba_offset.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), lba_offset());

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t DenaliInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write denali_endpoint.
  caps.WriteAt(next_cap, denali_endpoint());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), next_cap++);
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), device_id());
  // Write lba_offset.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), lba_offset());

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}


}  // namepace yellowstone
