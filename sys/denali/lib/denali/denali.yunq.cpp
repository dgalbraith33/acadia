// Generated file -- DO NOT MODIFY.
#include "denali.yunq.h"

#include <yunq/message_view.h>
#include <yunq/serialize.h>


namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status ReadRequest::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status ReadRequest::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status ReadRequest::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse device_id.
  ASSIGN_OR_RETURN(device_id_, message.ReadField<uint64_t>(0));
  // Parse lba.
  ASSIGN_OR_RETURN(lba_, message.ReadField<uint64_t>(1));
  // Parse size.
  ASSIGN_OR_RETURN(size_, message.ReadField<uint64_t>(2));

  return glcr::Status::Ok();
}

uint64_t ReadRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write lba.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), lba());
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), size());

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t ReadRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write lba.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), lba());
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), size());

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status ReadManyRequest::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status ReadManyRequest::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status ReadManyRequest::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse device_id.
  ASSIGN_OR_RETURN(device_id_, message.ReadField<uint64_t>(0));
  // Parse lba.
  ASSIGN_OR_RETURN(lba_, message.ReadRepeated<uint64_t>(1));

  // Parse sector_cnt.
  ASSIGN_OR_RETURN(sector_cnt_, message.ReadRepeated<uint64_t>(2));


  return glcr::Status::Ok();
}

uint64_t ReadManyRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write lba.
  ExtPointer lba_ptr{
    .offset = next_extension,
    .length = (uint32_t)(lba().size() * sizeof(uint64_t)),
  };

  next_extension += lba_ptr.length;
  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 1), lba_ptr);

  for (uint64_t i = 0; i < lba().size(); i++) {
    uint32_t ext_offset = offset + lba_ptr.offset + (i * sizeof(uint64_t));
    bytes.WriteAt<uint64_t>(ext_offset, lba().at(i));
  }
  // Write sector_cnt.
  ExtPointer sector_cnt_ptr{
    .offset = next_extension,
    .length = (uint32_t)(sector_cnt().size() * sizeof(uint64_t)),
  };

  next_extension += sector_cnt_ptr.length;
  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 2), sector_cnt_ptr);

  for (uint64_t i = 0; i < sector_cnt().size(); i++) {
    uint32_t ext_offset = offset + sector_cnt_ptr.offset + (i * sizeof(uint64_t));
    bytes.WriteAt<uint64_t>(ext_offset, sector_cnt().at(i));
  }

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t ReadManyRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write lba.
  ExtPointer lba_ptr{
    .offset = next_extension,
    .length = (uint32_t)(lba().size() * sizeof(uint64_t)),
  };

  next_extension += lba_ptr.length;
  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 1), lba_ptr);

  for (uint64_t i = 0; i < lba().size(); i++) {
    uint32_t ext_offset = offset + lba_ptr.offset + (i * sizeof(uint64_t));
    bytes.WriteAt<uint64_t>(ext_offset, lba().at(i));
  }
  // Write sector_cnt.
  ExtPointer sector_cnt_ptr{
    .offset = next_extension,
    .length = (uint32_t)(sector_cnt().size() * sizeof(uint64_t)),
  };

  next_extension += sector_cnt_ptr.length;
  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 2), sector_cnt_ptr);

  for (uint64_t i = 0; i < sector_cnt().size(); i++) {
    uint32_t ext_offset = offset + sector_cnt_ptr.offset + (i * sizeof(uint64_t));
    bytes.WriteAt<uint64_t>(ext_offset, sector_cnt().at(i));
  }

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status ReadResponse::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse memory.
  ASSIGN_OR_RETURN(memory_, message.ReadCapability(2));
  return glcr::Status::Ok();
}

glcr::Status ReadResponse::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse memory.
  ASSIGN_OR_RETURN(memory_, message.ReadCapability(2, caps));
  return glcr::Status::Ok();
}

glcr::Status ReadResponse::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse device_id.
  ASSIGN_OR_RETURN(device_id_, message.ReadField<uint64_t>(0));
  // Parse size.
  ASSIGN_OR_RETURN(size_, message.ReadField<uint64_t>(1));
  // Parse memory.

  return glcr::Status::Ok();
}

uint64_t ReadResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), size());
  // Write memory.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), 0);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t ReadResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), size());
  // Write memory.
  caps.WriteAt(next_cap, memory());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), next_cap++);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

