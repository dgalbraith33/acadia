// Generated file -- DO NOT MODIFY.
#include "example.yunq.h"

#include <yunq/message_view.h>
#include <yunq/serialize.h>


namespace srv::file {

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
  // Parse options.
  ASSIGN_OR_RETURN(options_, message.ReadRepeated<uint64_t>(1));


  return glcr::Status::Ok();
}

uint64_t OpenFileRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 2);
  return SerializeInternal(serializer);
}

uint64_t OpenFileRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 2, caps);
  return SerializeInternal(serializer);
}
  
uint64_t OpenFileRequest::SerializeInternal(yunq::Serializer& serializer) const {
  // Write path.
  serializer.WriteField<glcr::String>(0, path_);
  // Write options.
  serializer.WriteRepeated<uint64_t>(1, options_);

  serializer.WriteHeader();

  return serializer.size();
}
glcr::Status File::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse mem_cap.
  ASSIGN_OR_RETURN(mem_cap_, message.ReadCapability(2));
  return glcr::Status::Ok();
}

glcr::Status File::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse mem_cap.
  ASSIGN_OR_RETURN(mem_cap_, message.ReadCapability(2, caps));
  return glcr::Status::Ok();
}

glcr::Status File::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse path.
  ASSIGN_OR_RETURN(path_, message.ReadField<glcr::String>(0));
  // Parse attrs.
  ASSIGN_OR_RETURN(attrs_, message.ReadField<uint64_t>(1));
  // Parse mem_cap.

  return glcr::Status::Ok();
}

uint64_t File::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 3);
  return SerializeInternal(serializer);
}

uint64_t File::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 3, caps);
  return SerializeInternal(serializer);
}
  
uint64_t File::SerializeInternal(yunq::Serializer& serializer) const {
  // Write path.
  serializer.WriteField<glcr::String>(0, path_);
  // Write attrs.
  serializer.WriteField<uint64_t>(1, attrs_);
  // Write mem_cap.
  serializer.WriteCapability(2, mem_cap_);

  serializer.WriteHeader();

  return serializer.size();
}


}  // namepace srv::file
