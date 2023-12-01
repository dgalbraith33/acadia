// Generated file - DO NOT MODIFY
#include "denali.yunq.client.h"

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <mammoth/util/debug.h>
#include <zcall.h>




DenaliClient::~DenaliClient() {
  if (endpoint_ != 0) {
    check(ZCapRelease(endpoint_));
    }
}




glcr::Status DenaliClient::Read(const ReadRequest& request, ReadResponse& response) {

  uint64_t buffer_size = kBufferSize;
  uint64_t cap_size = kCapBufferSize;

  const uint32_t kSentinel = 0xBEEFDEAD;
  buffer_.WriteAt<uint32_t>(0, kSentinel);
  buffer_.WriteAt<uint64_t>(8, 0);

  cap_buffer_.Reset();

  uint64_t length = request.SerializeToBytes(buffer_, /*offset=*/16, cap_buffer_);


  buffer_.WriteAt<uint32_t>(4, 16 + length);

  z_cap_t reply_port_cap;
  RET_ERR(ZEndpointSend(endpoint_, 16 + length, buffer_.RawPtr(), cap_buffer_.UsedSlots(), cap_buffer_.RawPtr(), &reply_port_cap)); 

  // FIXME: Add a way to zero out the first buffer.
  RET_ERR(ZReplyPortRecv(reply_port_cap, &buffer_size, buffer_.RawPtr(), &cap_size, cap_buffer_.RawPtr()));

  if (buffer_.At<uint32_t>(0) != kSentinel) {
    return glcr::InvalidResponse("Got an invalid response from server.");
  }

  // Check Response Code.
  RET_ERR(buffer_.At<uint64_t>(8));


  RETURN_ERROR(response.ParseFromBytes(buffer_, 16, cap_buffer_));


  return glcr::OK;
}




glcr::Status DenaliClient::ReadMany(const ReadManyRequest& request, ReadResponse& response) {

  uint64_t buffer_size = kBufferSize;
  uint64_t cap_size = kCapBufferSize;

  const uint32_t kSentinel = 0xBEEFDEAD;
  buffer_.WriteAt<uint32_t>(0, kSentinel);
  buffer_.WriteAt<uint64_t>(8, 1);

  cap_buffer_.Reset();

  uint64_t length = request.SerializeToBytes(buffer_, /*offset=*/16, cap_buffer_);


  buffer_.WriteAt<uint32_t>(4, 16 + length);

  z_cap_t reply_port_cap;
  RET_ERR(ZEndpointSend(endpoint_, 16 + length, buffer_.RawPtr(), cap_buffer_.UsedSlots(), cap_buffer_.RawPtr(), &reply_port_cap)); 

  // FIXME: Add a way to zero out the first buffer.
  RET_ERR(ZReplyPortRecv(reply_port_cap, &buffer_size, buffer_.RawPtr(), &cap_size, cap_buffer_.RawPtr()));

  if (buffer_.At<uint32_t>(0) != kSentinel) {
    return glcr::InvalidResponse("Got an invalid response from server.");
  }

  // Check Response Code.
  RET_ERR(buffer_.At<uint64_t>(8));


  RETURN_ERROR(response.ParseFromBytes(buffer_, 16, cap_buffer_));


  return glcr::OK;
}




