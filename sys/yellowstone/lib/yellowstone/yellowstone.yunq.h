// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/string/string.h>
#include <ztypes.h>
class RegisterEndpointRequest {
 public:
  RegisterEndpointRequest() {}
  // Delete copy and move until implemented.
  RegisterEndpointRequest(const RegisterEndpointRequest&) = delete;
  RegisterEndpointRequest(RegisterEndpointRequest&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const;
  glcr::String endpoint_name() const { return endpoint_name_; }
  void set_endpoint_name(const glcr::String& value) { endpoint_name_ = value; }
  z_cap_t endpoint_capability() const { return endpoint_capability_; }
  void set_endpoint_capability(const z_cap_t& value) { endpoint_capability_ = value; }

 private:
  glcr::String endpoint_name_;
  z_cap_t endpoint_capability_;

};
class Empty {
 public:
  Empty() {}
  // Delete copy and move until implemented.
  Empty(const Empty&) = delete;
  Empty(Empty&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const;

 private:

};
class AhciInfo {
 public:
  AhciInfo() {}
  // Delete copy and move until implemented.
  AhciInfo(const AhciInfo&) = delete;
  AhciInfo(AhciInfo&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const;
  z_cap_t ahci_region() const { return ahci_region_; }
  void set_ahci_region(const z_cap_t& value) { ahci_region_ = value; }
  uint64_t region_length() const { return region_length_; }
  void set_region_length(const uint64_t& value) { region_length_ = value; }

 private:
  z_cap_t ahci_region_;
  uint64_t region_length_;

};
class DenaliInfo {
 public:
  DenaliInfo() {}
  // Delete copy and move until implemented.
  DenaliInfo(const DenaliInfo&) = delete;
  DenaliInfo(DenaliInfo&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const;
  z_cap_t denali_endpoint() const { return denali_endpoint_; }
  void set_denali_endpoint(const z_cap_t& value) { denali_endpoint_ = value; }
  uint64_t device_id() const { return device_id_; }
  void set_device_id(const uint64_t& value) { device_id_ = value; }
  uint64_t lba_offset() const { return lba_offset_; }
  void set_lba_offset(const uint64_t& value) { lba_offset_ = value; }

 private:
  z_cap_t denali_endpoint_;
  uint64_t device_id_;
  uint64_t lba_offset_;

};