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
glcr::Status DiskBlock::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status DiskBlock::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status DiskBlock::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse lba.
  ASSIGN_OR_RETURN(lba_, message.ReadField<uint64_t>(0));
  // Parse size.
  ASSIGN_OR_RETURN(size_, message.ReadField<uint64_t>(1));

  return glcr::Status::Ok();
}

uint64_t DiskBlock::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 2);
  return SerializeInternal(serializer);
}

uint64_t DiskBlock::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 2, caps);
  return SerializeInternal(serializer);
}
  
uint64_t DiskBlock::SerializeInternal(yunq::Serializer& serializer) const {
  // Write lba.
  serializer.WriteField<uint64_t>(0, lba_);
  // Write size.
  serializer.WriteField<uint64_t>(1, size_);

  serializer.WriteHeader();

  return serializer.size();
}
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
  // Parse block.
  message.ReadMessage<DiskBlock>(1, block_);

  return glcr::Status::Ok();
}

uint64_t ReadRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 2);
  return SerializeInternal(serializer);
}

uint64_t ReadRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 2, caps);
  return SerializeInternal(serializer);
}
  
uint64_t ReadRequest::SerializeInternal(yunq::Serializer& serializer) const {
  // Write device_id.
  serializer.WriteField<uint64_t>(0, device_id_);
  // Write block.
  serializer.WriteMessage<DiskBlock>(1, block_);

  serializer.WriteHeader();

  return serializer.size();
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
  yunq::Serializer serializer(bytes, offset, 3);
  return SerializeInternal(serializer);
}

uint64_t ReadManyRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 3, caps);
  return SerializeInternal(serializer);
}
  
uint64_t ReadManyRequest::SerializeInternal(yunq::Serializer& serializer) const {
  // Write device_id.
  serializer.WriteField<uint64_t>(0, device_id_);
  // Write lba.
  serializer.WriteRepeated<uint64_t>(1, lba_);
  // Write sector_cnt.
  serializer.WriteRepeated<uint64_t>(2, sector_cnt_);

  serializer.WriteHeader();

  return serializer.size();
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
  yunq::Serializer serializer(bytes, offset, 3);
  return SerializeInternal(serializer);
}

uint64_t ReadResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 3, caps);
  return SerializeInternal(serializer);
}
  
uint64_t ReadResponse::SerializeInternal(yunq::Serializer& serializer) const {
  // Write device_id.
  serializer.WriteField<uint64_t>(0, device_id_);
  // Write size.
  serializer.WriteField<uint64_t>(1, size_);
  // Write memory.
  serializer.WriteCapability(2, memory_);

  serializer.WriteHeader();

  return serializer.size();
}

