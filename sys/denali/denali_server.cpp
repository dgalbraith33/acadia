#include "denali_server.h"

#include <glacier/memory/move.h>
#include <glacier/status/error.h>
#include <mammoth/debug.h>
#include <zcall.h>

namespace {
DenaliServer* gServer = nullptr;
void HandleResponse(z_cap_t reply_port, uint64_t lba, uint64_t size,
                    z_cap_t mem) {
  gServer->HandleResponse(reply_port, lba, size, mem);
}
}  // namespace

DenaliServer::DenaliServer(glcr::UniquePtr<EndpointServer> server,
                           AhciDriver& driver)
    : server_(glcr::Move(server)), driver_(driver) {
  gServer = this;
}

glcr::ErrorCode DenaliServer::RunServer() {
  while (true) {
    uint64_t buff_size = kBuffSize;
    z_cap_t reply_port;
    RET_ERR(server_->Recieve(&buff_size, read_buffer_, &reply_port));
    if (buff_size < sizeof(uint64_t)) {
      dbgln("Skipping invalid message");
      continue;
    }
    uint64_t type = *reinterpret_cast<uint64_t*>(read_buffer_);
    switch (type) {
      case Z_INVALID:
        dbgln(reinterpret_cast<char*>(read_buffer_));
        break;
      case DENALI_READ: {
        DenaliRead* read_req = reinterpret_cast<DenaliRead*>(read_buffer_);
        uint64_t memcap = 0;
        RET_ERR(HandleRead(*read_req, reply_port));
        break;
      }
      default:
        dbgln("Invalid message type.");
        return glcr::UNIMPLEMENTED;
    }
  }
}

glcr::ErrorCode DenaliServer::HandleRead(const DenaliRead& read,
                                         z_cap_t reply_port) {
  ASSIGN_OR_RETURN(AhciDevice * device, driver_.GetDevice(read.device_id));

  device->IssueCommand(
      new DmaReadCommand(read.lba, read.size, ::HandleResponse, reply_port));

  return glcr::OK;
}

void DenaliServer::HandleResponse(z_cap_t reply_port, uint64_t lba,
                                  uint64_t size, z_cap_t mem) {
  DenaliReadResponse resp{
      .device_id = 0,
      .lba = lba,
      .size = size,
  };
  check(ZReplyPortSend(reply_port, sizeof(resp), &resp, 1, &mem));
}
