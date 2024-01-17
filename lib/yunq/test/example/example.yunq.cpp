// Generated file -- DO NOT MODIFY.
#include "example.yunq.h"

#include <yunq/message_view.h>
#include <yunq/serialize.h>


namespace ex {

namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status Basic::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Basic::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Basic::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse field.
  ASSIGN_OR_RETURN(field_, message.ReadField<uint64_t>(0));

  return glcr::Status::Ok();
}

uint64_t Basic::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t Basic::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t Basic::SerializeInternal(yunq::Serializer& serializer) const {
  // Write field.
  serializer.WriteField<uint64_t>(0, field_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status Types::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Types::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Types::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse unsigned_int.
  ASSIGN_OR_RETURN(unsigned_int_, message.ReadField<uint64_t>(0));
  // Parse signed_int.
  ASSIGN_OR_RETURN(signed_int_, message.ReadField<int64_t>(1));
  // Parse str.
  ASSIGN_OR_RETURN(str_, message.ReadField<glcr::String>(2));

  return glcr::Status::Ok();
}

uint64_t Types::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 3);
  return SerializeInternal(serializer);
}

uint64_t Types::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 3, caps);
  return SerializeInternal(serializer);
}
  
uint64_t Types::SerializeInternal(yunq::Serializer& serializer) const {
  // Write unsigned_int.
  serializer.WriteField<uint64_t>(0, unsigned_int_);
  // Write signed_int.
  serializer.WriteField<int64_t>(1, signed_int_);
  // Write str.
  serializer.WriteField<glcr::String>(2, str_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status Cap::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse cap.
  ASSIGN_OR_RETURN(cap_, message.ReadCapability(0));
  return glcr::Status::Ok();
}

glcr::Status Cap::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse cap.
  ASSIGN_OR_RETURN(cap_, message.ReadCapability(0, caps));
  return glcr::Status::Ok();
}

glcr::Status Cap::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse cap.

  return glcr::Status::Ok();
}

uint64_t Cap::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t Cap::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t Cap::SerializeInternal(yunq::Serializer& serializer) const {
  // Write cap.
  serializer.WriteCapability(0, cap_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status Repeated::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Repeated::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Repeated::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse unsigned_ints.
  ASSIGN_OR_RETURN(unsigned_ints_, message.ReadRepeated<uint64_t>(0));


  return glcr::Status::Ok();
}

uint64_t Repeated::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t Repeated::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t Repeated::SerializeInternal(yunq::Serializer& serializer) const {
  // Write unsigned_ints.
  serializer.WriteRepeated<uint64_t>(0, unsigned_ints_);

  serializer.WriteHeader();

  return serializer.size();
}


}  // namepace ex
