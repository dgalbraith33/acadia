#include "denali_server.h"

#include <glacier/status/error.h>
#include <mammoth/debug.h>
#include <zcall.h>

namespace {
DenaliServer* gServer = nullptr;
void HandleResponse(uint64_t lba, uint64_t size, uint64_t cap) {
  gServer->HandleResponse(lba, size, cap);
}
}  // namespace

DenaliServer::DenaliServer(uint64_t channel_cap, AhciDriver& driver)
    : channel_cap_(channel_cap), driver_(driver) {
  gServer = this;
}

glcr::ErrorCode DenaliServer::RunServer() {
  while (true) {
    uint64_t buff_size = kBuffSize;
    uint64_t cap_size = 0;
    RET_ERR(ZChannelRecv(channel_cap_, &buff_size, read_buffer_, &cap_size,
                         nullptr));
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
        RET_ERR(HandleRead(*read_req));
        break;
      }
      default:
        dbgln("Invalid message type.");
        return glcr::UNIMPLEMENTED;
    }
  }
}

glcr::ErrorCode DenaliServer::HandleRead(const DenaliRead& read) {
  ASSIGN_OR_RETURN(AhciDevice * device, driver_.GetDevice(read.device_id));

  device->IssueCommand(
      new DmaReadCommand(read.lba, read.size, ::HandleResponse));

  return glcr::OK;
}

void DenaliServer::HandleResponse(uint64_t lba, uint64_t size, uint64_t cap) {
  DenaliReadResponse resp{
      .device_id = 0,
      .lba = lba,
      .size = size,
  };
  check(ZChannelSend(channel_cap_, sizeof(resp),
                     reinterpret_cast<uint8_t*>(&resp), 1, &cap));
}
