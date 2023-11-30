#include "file/file.h"

#include <glacier/string/str_split.h>
#include <victoriafalls/victoriafalls.yunq.client.h>
#include <yellowstone/yellowstone.yunq.client.h>
#include <zcall.h>
#include <zglobal.h>

#include "util/debug.h"

namespace mmth {
namespace {

using yellowstone::Endpoint;
using yellowstone::GetEndpointRequest;
using yellowstone::YellowstoneClient;

VFSClient* gVfsClient = nullptr;

void GetVfsClientIfNeeded() {
  if (gVfsClient == nullptr) {
    // TODO: Add an unowned client so we don't have to duplicate this cap every
    // time.
    uint64_t dup_cap;
    check(ZCapDuplicate(gInitEndpointCap, kZionPerm_All, &dup_cap));
    YellowstoneClient client(dup_cap);

    GetEndpointRequest yreq;
    yreq.set_endpoint_name("victoriafalls");
    Endpoint yresp;
    check(client.GetEndpoint(yreq, yresp));

    gVfsClient = new VFSClient(yresp.endpoint());
  }
}

}  // namespace

void SetVfsCap(z_cap_t vfs_cap) { gVfsClient = new VFSClient(vfs_cap); }

File File::Open(glcr::StringView path) {
  GetVfsClientIfNeeded();

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

glcr::ErrorOr<glcr::Vector<glcr::String>> ListDirectory(glcr::StringView path) {
  GetVfsClientIfNeeded();

  GetDirectoryRequest req;
  req.set_path(path);
  Directory dir;
  RET_ERR(gVfsClient->GetDirectory(req, dir));

  auto file_views = glcr::StrSplit(dir.filenames(), ',');
  glcr::Vector<glcr::String> files;
  for (uint64_t i = 0; i < file_views.size(); i++) {
    files.PushBack(file_views[i]);
  }
  return files;
}

}  // namespace mmth
