// Generated file -- DO NOT MODIFY.
#include "example.yunq.server.h"

#include <mammoth/debug.h>
#include <zcall.h>

namespace {

const uint32_t kSentinel = 0xBEEFDEAD;
const uint32_t kHeaderSize = 0x10;

void WriteError(glcr::ByteBuffer& buffer, glcr::ErrorCode err) {
  buffer.WriteAt<uint32_t>(0, kSentinel);
  buffer.WriteAt<uint32_t>(4, kHeaderSize);
  buffer.WriteAt<uint64_t>(8, err); 
}

void WriteHeader(glcr::ByteBuffer& buffer, uint64_t message_length) {
  buffer.WriteAt<uint32_t>(0, kSentinel);
  buffer.WriteAt<uint32_t>(4, kHeaderSize + message_length);
  buffer.WriteAt<uint64_t>(8, glcr::OK); 
}

}  // namespace



void VFSServerBaseThreadBootstrap(void* server_base) {
  ((VFSServerBase*)server_base)->ServerThread();
}

glcr::ErrorOr<VFSClient> VFSServerBase::CreateClient() {
  uint64_t client_cap;
  // FIXME: Restrict permissions to send-only here.
  RET_ERR(ZCapDuplicate(endpoint_, &client_cap));
  return VFSClient(client_cap);
}

Thread VFSServerBase::RunServer() {
  return Thread(VFSServerBaseThreadBootstrap, this);
}

void VFSServerBase::ServerThread() {
  glcr::ByteBuffer recv_buffer(0x1000);
  glcr::CapBuffer recv_cap(0x10);
  glcr::ByteBuffer resp_buffer(0x1000);
  glcr::CapBuffer resp_cap(0x10);
  z_cap_t reply_port_cap;

  while (true) {
    uint64_t recv_cap_size = 0x10;
    uint64_t recv_buf_size = 0x1000;
    glcr::ErrorCode recv_err = ZEndpointRecv(endpoint_, &recv_buf_size, recv_buffer.RawPtr(), &recv_cap_size, recv_cap.RawPtr(), &reply_port_cap);
    if (recv_err != glcr::OK) {
      dbgln("Error in receive: %x", recv_err);
      continue;
    }

    uint64_t resp_length = 0;
    
    glcr::ErrorCode reply_err = glcr::OK;
    glcr::ErrorCode err = HandleRequest(recv_buffer, recv_cap, resp_buffer, resp_length, resp_cap);
    if (err != glcr::OK) {
      WriteError(resp_buffer, err);
      reply_err = ZReplyPortSend(reply_port_cap, kHeaderSize, resp_buffer.RawPtr(), 0, nullptr);
    } else {
      WriteHeader(resp_buffer, resp_length);
      reply_err = ZReplyPortSend(reply_port_cap, kHeaderSize + resp_length, resp_buffer.RawPtr(), resp_cap.UsedSlots(), resp_cap.RawPtr());
    }
    if (reply_err != glcr::OK) {
      dbgln("Error in reply: %x", reply_err);
    }
  }

}

glcr::ErrorCode VFSServerBase::HandleRequest(const glcr::ByteBuffer& request,
                                                            const glcr::CapBuffer& req_caps,
                                                            glcr::ByteBuffer& response, uint64_t& resp_length,
                                                            glcr::CapBuffer& resp_caps) {
  if (request.At<uint32_t>(0) != kSentinel) {
    return glcr::INVALID_ARGUMENT;
  } 
  
  uint64_t method_select = request.At<uint64_t>(8);

  switch(method_select) {
    case 0: {
      OpenFileRequest yunq_request;
      File yunq_response;

      yunq_request.ParseFromBytes(request, kHeaderSize, req_caps);

      RET_ERR(Handleopen(yunq_request, yunq_response));

      resp_length = yunq_response.SerializeToBytes(response, kHeaderSize, resp_caps);
      break;
    }
    default: {
      return glcr::UNIMPLEMENTED;
    }
  }
  return glcr::OK;
}
