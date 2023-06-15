#include "denali_server.h"

#include <mammoth/debug.h>
#include <zcall.h>

DenaliServer::DenaliServer(uint64_t channel_cap, AhciDriver& driver)
    : channel_cap_(channel_cap), driver_(driver) {}

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
        DenaliReadResponse resp;
        RET_ERR(HandleRead(*read_req, resp, memcap));
        uint64_t caps_len = memcap ? 1 : 0;
        RET_ERR(ZChannelSend(channel_cap_, 0, sizeof(DenaliReadResponse),
                             reinterpret_cast<uint8_t*>(&resp), caps_len,
                             &memcap));
        break;
      }
      default:
        dbgln("Invalid message type.");
        return Z_ERR_UNIMPLEMENTED;
    }
  }
}

z_err_t DenaliServer::HandleRead(const DenaliRead& read,
                                 DenaliReadResponse& resp, uint64_t& memcap) {
  AhciDevice device;
  RET_ERR(driver_.GetDevice(read.device_id, device));

  return Z_OK;
}
