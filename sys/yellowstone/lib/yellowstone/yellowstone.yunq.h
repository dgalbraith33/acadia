// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/container/vector.h>
#include <glacier/string/string.h>
#include <ztypes.h>


namespace yellowstone {

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
  const glcr::String& endpoint_name() const { return endpoint_name_; }
  void set_endpoint_name(const glcr::String& value) { endpoint_name_ = value; } 
  const z_cap_t& endpoint_capability() const { return endpoint_capability_; }
  void set_endpoint_capability(const z_cap_t& value) { endpoint_capability_ = value; }

 private:
  glcr::String endpoint_name_;
  z_cap_t endpoint_capability_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class GetEndpointRequest {
 public:
  GetEndpointRequest() {}
  // Delete copy and move until implemented.
  GetEndpointRequest(const GetEndpointRequest&) = delete;
  GetEndpointRequest(GetEndpointRequest&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  const glcr::String& endpoint_name() const { return endpoint_name_; }
  void set_endpoint_name(const glcr::String& value) { endpoint_name_ = value; }

 private:
  glcr::String endpoint_name_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class Endpoint {
 public:
  Endpoint() {}
  // Delete copy and move until implemented.
  Endpoint(const Endpoint&) = delete;
  Endpoint(Endpoint&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  const z_cap_t& endpoint() const { return endpoint_; }
  void set_endpoint(const z_cap_t& value) { endpoint_ = value; }

 private:
  z_cap_t endpoint_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
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
  const z_cap_t& ahci_region() const { return ahci_region_; }
  void set_ahci_region(const z_cap_t& value) { ahci_region_ = value; } 
  const uint64_t& region_length() const { return region_length_; }
  void set_region_length(const uint64_t& value) { region_length_ = value; }

 private:
  z_cap_t ahci_region_;
  uint64_t region_length_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class FramebufferInfo {
 public:
  FramebufferInfo() {}
  // Delete copy and move until implemented.
  FramebufferInfo(const FramebufferInfo&) = delete;
  FramebufferInfo(FramebufferInfo&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  const uint64_t& address_phys() const { return address_phys_; }
  void set_address_phys(const uint64_t& value) { address_phys_ = value; } 
  const uint64_t& width() const { return width_; }
  void set_width(const uint64_t& value) { width_ = value; } 
  const uint64_t& height() const { return height_; }
  void set_height(const uint64_t& value) { height_ = value; } 
  const uint64_t& pitch() const { return pitch_; }
  void set_pitch(const uint64_t& value) { pitch_ = value; } 
  const uint64_t& bpp() const { return bpp_; }
  void set_bpp(const uint64_t& value) { bpp_ = value; } 
  const uint64_t& memory_model() const { return memory_model_; }
  void set_memory_model(const uint64_t& value) { memory_model_ = value; } 
  const uint64_t& red_mask_size() const { return red_mask_size_; }
  void set_red_mask_size(const uint64_t& value) { red_mask_size_ = value; } 
  const uint64_t& red_mask_shift() const { return red_mask_shift_; }
  void set_red_mask_shift(const uint64_t& value) { red_mask_shift_ = value; } 
  const uint64_t& green_mask_size() const { return green_mask_size_; }
  void set_green_mask_size(const uint64_t& value) { green_mask_size_ = value; } 
  const uint64_t& green_mask_shift() const { return green_mask_shift_; }
  void set_green_mask_shift(const uint64_t& value) { green_mask_shift_ = value; } 
  const uint64_t& blue_mask_size() const { return blue_mask_size_; }
  void set_blue_mask_size(const uint64_t& value) { blue_mask_size_ = value; } 
  const uint64_t& blue_mask_shift() const { return blue_mask_shift_; }
  void set_blue_mask_shift(const uint64_t& value) { blue_mask_shift_ = value; }

 private:
  uint64_t address_phys_;
  uint64_t width_;
  uint64_t height_;
  uint64_t pitch_;
  uint64_t bpp_;
  uint64_t memory_model_;
  uint64_t red_mask_size_;
  uint64_t red_mask_shift_;
  uint64_t green_mask_size_;
  uint64_t green_mask_shift_;
  uint64_t blue_mask_size_;
  uint64_t blue_mask_shift_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
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
  const z_cap_t& denali_endpoint() const { return denali_endpoint_; }
  void set_denali_endpoint(const z_cap_t& value) { denali_endpoint_ = value; } 
  const uint64_t& device_id() const { return device_id_; }
  void set_device_id(const uint64_t& value) { device_id_ = value; } 
  const uint64_t& lba_offset() const { return lba_offset_; }
  void set_lba_offset(const uint64_t& value) { lba_offset_ = value; }

 private:
  z_cap_t denali_endpoint_;
  uint64_t device_id_;
  uint64_t lba_offset_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};


}  // namepace yellowstone
