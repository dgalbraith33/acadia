// Generated file -- DO NOT MODIFY.
#include "yellowstone.yunq.server.h"

#include <mammoth/util/debug.h>
#include <zcall.h>


namespace yellowstone {

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



void YellowstoneServerBaseThreadBootstrap(void* server_base) {
  ((YellowstoneServerBase*)server_base)->ServerThread();
}

YellowstoneServerBase::~YellowstoneServerBase() {
  if (endpoint_ != 0) {
    check(ZCapRelease(endpoint_));
  }
}

glcr::ErrorOr<z_cap_t> YellowstoneServerBase::CreateClientCap() {
  uint64_t client_cap;
  RET_ERR(ZCapDuplicate(endpoint_, ~(kZionPerm_Read), &client_cap));
  return client_cap;
}

glcr::ErrorOr<YellowstoneClient> YellowstoneServerBase::CreateClient() {
  uint64_t client_cap;
  RET_ERR(ZCapDuplicate(endpoint_, ~(kZionPerm_Read), &client_cap));
  return YellowstoneClient(client_cap);
}

Thread YellowstoneServerBase::RunServer() {
  return Thread(YellowstoneServerBaseThreadBootstrap, this);
}

void YellowstoneServerBase::ServerThread() {
  glcr::ByteBuffer recv_buffer(0x1000);
  glcr::CapBuffer recv_cap(0x10);
  glcr::ByteBuffer resp_buffer(0x1000);
  glcr::CapBuffer resp_cap(0x10);
  z_cap_t reply_port_cap;

  while (true) {
    uint64_t recv_cap_size = 0x10;
    uint64_t recv_buf_size = 0x1000;
    recv_cap.Reset();
    glcr::ErrorCode recv_err = static_cast<glcr::ErrorCode>(ZEndpointRecv(endpoint_, &recv_buf_size, recv_buffer.RawPtr(), &recv_cap_size, recv_cap.RawPtr(), &reply_port_cap));
    if (recv_err != glcr::OK) {
      dbgln("Error in receive: {x}", recv_err);
      continue;
    }

    uint64_t resp_length = 0;
    
    glcr::ErrorCode reply_err = glcr::OK;
    resp_cap.Reset();
    glcr::Status err = HandleRequest(recv_buffer, recv_cap, resp_buffer, resp_length, resp_cap);
    if (!err) {
      WriteError(resp_buffer, err.code());
      dbgln("Responding Error {}", err.message());
      reply_err = static_cast<glcr::ErrorCode>(ZReplyPortSend(reply_port_cap, kHeaderSize, resp_buffer.RawPtr(), 0, nullptr));
    } else {
      WriteHeader(resp_buffer, resp_length);
      reply_err = static_cast<glcr::ErrorCode>(ZReplyPortSend(reply_port_cap, kHeaderSize + resp_length, resp_buffer.RawPtr(), resp_cap.UsedSlots(), resp_cap.RawPtr()));
    }
    if (reply_err != glcr::OK) {
      dbgln("Error in reply: {x}", reply_err);
    }
  }

}

glcr::Status YellowstoneServerBase::HandleRequest(const glcr::ByteBuffer& request,
                                                         const glcr::CapBuffer& req_caps,
                                                         glcr::ByteBuffer& response, uint64_t& resp_length,
                                                         glcr::CapBuffer& resp_caps) {
  if (request.At<uint32_t>(0) != kSentinel) {
    return glcr::InvalidArgument("Request Not Valid");
  } 
  
  uint64_t method_select = request.At<uint64_t>(8);

  switch(method_select) {
    case 0: {

  
      RegisterEndpointRequest yunq_request;
      yunq::MessageView request_view(request, kHeaderSize);
      RETURN_ERROR(yunq_request.ParseFromBytes(request_view, req_caps));
  

  

  
      RETURN_ERROR(HandleRegisterEndpoint(yunq_request));
  

  
      resp_length = 0;
  
      break;
    }
    case 1: {

  
      GetEndpointRequest yunq_request;
      yunq::MessageView request_view(request, kHeaderSize);
      RETURN_ERROR(yunq_request.ParseFromBytes(request_view, req_caps));
  

  
      Endpoint yunq_response;
  

  
      RETURN_ERROR(HandleGetEndpoint(yunq_request, yunq_response));
  

  
      resp_length = yunq_response.SerializeToBytes(response, kHeaderSize, resp_caps);
  
      break;
    }
    case 2: {

  

  
      AhciInfo yunq_response;
  

  
      RETURN_ERROR(HandleGetAhciInfo(yunq_response));
  

  
      resp_length = yunq_response.SerializeToBytes(response, kHeaderSize, resp_caps);
  
      break;
    }
    case 3: {

  

  
      XhciInfo yunq_response;
  

  
      RETURN_ERROR(HandleGetXhciInfo(yunq_response));
  

  
      resp_length = yunq_response.SerializeToBytes(response, kHeaderSize, resp_caps);
  
      break;
    }
    case 4: {

  

  
      FramebufferInfo yunq_response;
  

  
      RETURN_ERROR(HandleGetFramebufferInfo(yunq_response));
  

  
      resp_length = yunq_response.SerializeToBytes(response, kHeaderSize, resp_caps);
  
      break;
    }
    case 5: {

  

  
      DenaliInfo yunq_response;
  

  
      RETURN_ERROR(HandleGetDenali(yunq_response));
  

  
      resp_length = yunq_response.SerializeToBytes(response, kHeaderSize, resp_caps);
  
      break;
    }
    default: {
      return glcr::Unimplemented("Method unimplemented by server.");
    }
  }
  return glcr::Status::Ok();
}



}  // namepace yellowstone
