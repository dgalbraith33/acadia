// Generated file - DO NOT MODIFY
#include "example.yunq.client.h"

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>




glcr::ErrorCode open(const OpenFileRequest& request, File& response) {
  uint64_t buffer_size = 0x1000;
  // FIXME: Maybe raise this limit.
  uint64_t cap_size = 100;
  glcr::ByteBuffer buffer(buffer_size);
  glcr::CapBuffer caps(cap_size);

  const uint32_t kSentinel = 0xBEEFDEAD;
  buffer.WriteAt<uint32_t>(0, kSentinel);
  buffer.WriteAt<uint64_t>(8, {method_number});

  uint64_t length = request.SerializeToBytes(buffer, /*offset=*/16, caps);
  buffer.WriteAt<uint32_t>(4, 16 + length);

  z_cap_t reply_port_cap;
  // FIXME: We need to be able to send capabilities via endpoint call.
  RET_ERR(ZEndpointSend(endpoint_, 16 + length, buffer.RawPtr())); 

  // FIXME: Add a way to zero out the first buffer.
  glcr::ByteBuffer recv_buffer(buffer_size);
  glcr::CapBuffer recv_caps(cap_size);
  RET_ERR(ZReplyPortRecv(reply_port_cap, &buffer_size, recv_buffer.RawPtr(), &cap_size, recv_caps.RawPtr()));

  if (recv_buffer.At<uint32_t>(0) != kSentinel) {
    return glcr::INVALID_RESPONSE;
  }

  // Check Response Code.
  RET_ERR(recv_buffer.At<uint64_t>(8));

  response.ParseFromBytes(recv_buffer, 16, recv_caps);

  return glcr::OK;
}


