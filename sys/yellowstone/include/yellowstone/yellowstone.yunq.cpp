// Generated file -- DO NOT MODIFY.
#include "yellowstone.yunq.h"

namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

void CheckHeader(const glcr::ByteBuffer& bytes) {
  // TODO: Check ident.
  // TODO: Parse core size.
  // TODO: Parse extension size.
  // TODO: Check CRC32
  // TODO: Parse options.
}

void WriteHeader(glcr::ByteBuffer& bytes, uint64_t offset, uint32_t core_size, uint32_t extension_size) {
  bytes.WriteAt<uint32_t>(offset + 0, 0xDEADBEEF);  // TODO: Chose a more unique ident sequence.
  bytes.WriteAt<uint32_t>(offset + 4, core_size);
  bytes.WriteAt<uint32_t>(offset + 8, extension_size);
  bytes.WriteAt<uint32_t>(offset + 12, 0); // TODO: Calculate CRC32.
  bytes.WriteAt<uint64_t>(offset + 16, 0); // TODO: Add options.
}

}  // namespace
void Empty::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
}

void Empty::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);
}

uint64_t Empty::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 0;
  const uint32_t core_size = next_extension;

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t Empty::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 0;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
void RegisterInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse register_port.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_register_port(0);
}

void RegisterInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);
  // Parse register_port.
  uint64_t register_port_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 0));

  set_register_port(caps.At(register_port_ptr));
}

uint64_t RegisterInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  // Write register_port.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), 0);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t RegisterInfo::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write register_port.
  caps.WriteAt(next_cap, register_port());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), next_cap++);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
void AhciInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse ahci_region.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_ahci_region(0);
  // Parse region_length.
  set_region_length(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
}

void AhciInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);
  // Parse ahci_region.
  uint64_t ahci_region_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 0));

  set_ahci_region(caps.At(ahci_region_ptr));
  // Parse region_length.
  set_region_length(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
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
  WriteHeader(bytes, offset, core_size, next_extension);

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
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
void DenaliInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse denali_endpoint.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_denali_endpoint(0);
  // Parse device_id.
  set_device_id(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse lba_offset.
  set_lba_offset(bytes.At<uint64_t>(offset + header_size + (8 * 2)));
}

void DenaliInfo::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);
  // Parse denali_endpoint.
  uint64_t denali_endpoint_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 0));

  set_denali_endpoint(caps.At(denali_endpoint_ptr));
  // Parse device_id.
  set_device_id(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse lba_offset.
  set_lba_offset(bytes.At<uint64_t>(offset + header_size + (8 * 2)));
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
  WriteHeader(bytes, offset, core_size, next_extension);

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
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}