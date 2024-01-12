// Generated file -- DO NOT MODIFY.
#include "victoriafalls.yunq.h"

#include <yunq/message_view.h>
#include <yunq/serialize.h>


namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status OpenFileRequest::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status OpenFileRequest::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status OpenFileRequest::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse path.
  ASSIGN_OR_RETURN(path_, message.ReadField<glcr::String>(0));

  return glcr::Status::Ok();
}

uint64_t OpenFileRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t OpenFileRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t OpenFileRequest::SerializeInternal(yunq::Serializer& serializer) const {
  // Write path.
  serializer.WriteField<glcr::String>(0, path_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status OpenFileResponse::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse memory.
  ASSIGN_OR_RETURN(memory_, message.ReadCapability(2));
  return glcr::Status::Ok();
}

glcr::Status OpenFileResponse::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse memory.
  ASSIGN_OR_RETURN(memory_, message.ReadCapability(2, caps));
  return glcr::Status::Ok();
}

glcr::Status OpenFileResponse::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse path.
  ASSIGN_OR_RETURN(path_, message.ReadField<glcr::String>(0));
  // Parse size.
  ASSIGN_OR_RETURN(size_, message.ReadField<uint64_t>(1));
  // Parse memory.

  return glcr::Status::Ok();
}

uint64_t OpenFileResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 3);
  return SerializeInternal(serializer);
}

uint64_t OpenFileResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 3, caps);
  return SerializeInternal(serializer);
}
  
uint64_t OpenFileResponse::SerializeInternal(yunq::Serializer& serializer) const {
  // Write path.
  serializer.WriteField<glcr::String>(0, path_);
  // Write size.
  serializer.WriteField<uint64_t>(1, size_);
  // Write memory.
  serializer.WriteCapability(2, memory_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status GetDirectoryRequest::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status GetDirectoryRequest::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status GetDirectoryRequest::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse path.
  ASSIGN_OR_RETURN(path_, message.ReadField<glcr::String>(0));

  return glcr::Status::Ok();
}

uint64_t GetDirectoryRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t GetDirectoryRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t GetDirectoryRequest::SerializeInternal(yunq::Serializer& serializer) const {
  // Write path.
  serializer.WriteField<glcr::String>(0, path_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status Directory::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Directory::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Directory::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse filenames.
  ASSIGN_OR_RETURN(filenames_, message.ReadField<glcr::String>(0));

  return glcr::Status::Ok();
}

uint64_t Directory::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t Directory::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t Directory::SerializeInternal(yunq::Serializer& serializer) const {
  // Write filenames.
  serializer.WriteField<glcr::String>(0, filenames_);

  serializer.WriteHeader();

  return serializer.size();
}

