#include "file/file.h"

#include <victoriafalls/victoriafalls.yunq.client.h>
#include <yellowstone/yellowstone.yunq.client.h>
#include <zglobal.h>

#include "util/debug.h"

namespace mmth {
namespace {

VFSClient* gVfsClient = nullptr;

}  // namespace

void SetVfsCap(z_cap_t vfs_cap) { gVfsClient = new VFSClient(vfs_cap); }

File File::Open(glcr::StringView path) {
  if (gVfsClient == 0) {
    YellowstoneClient client(gInitEndpointCap);

    GetEndpointRequest yreq;
    yreq.set_endpoint_name("victoriafalls");
    Endpoint yresp;
    check(client.GetEndpoint(yreq, yresp));

    gVfsClient = new VFSClient(yresp.endpoint());
  }

  OpenFileRequest req;
  req.set_path(path);
  OpenFileResponse resp;
  check(gVfsClient->OpenFile(req, resp));

  return File(OwnedMemoryRegion::FromCapability(resp.memory()), resp.size());
}

glcr::StringView File::as_str() {
  return glcr::StringView((char*)raw_ptr(), size_);
}

void* File::raw_ptr() { return reinterpret_cast<void*>(file_data_.vaddr()); }
uint8_t* File::byte_ptr() {
  return reinterpret_cast<uint8_t*>(file_data_.vaddr());
}

}  // namespace mmth
