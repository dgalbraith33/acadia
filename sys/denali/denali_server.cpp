#include "denali_server.h"

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

z_err_t DenaliServer::RunServer() {
  while (true) {
    uint64_t buff_size = kBuffSize;
    uint64_t cap_size = 0;
    uint64_t type = DENALI_INVALID;
    RET_ERR(ZChannelRecv(channel_cap_, buff_size, read_buffer_, 0, nullptr,
                         &type, &buff_size, &cap_size));
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
        return Z_ERR_UNIMPLEMENTED;
    }
  }
}

z_err_t DenaliServer::HandleRead(const DenaliRead& read) {
  AhciDevice* device;
  RET_ERR(driver_.GetDevice(read.device_id, &device));

  device->IssueCommand(
      new DmaReadCommand(read.lba, read.size, ::HandleResponse));

  return Z_OK;
}

void DenaliServer::HandleResponse(uint64_t lba, uint64_t size, uint64_t cap) {
  DenaliReadResponse resp{
      .device_id = 0,
      .lba = lba,
      .size = size,
  };
  check(ZChannelSend(channel_cap_, DENALI_READ, sizeof(resp),
                     reinterpret_cast<uint8_t*>(&resp), 1, &cap));
}
