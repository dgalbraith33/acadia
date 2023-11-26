#include "victoriafalls_server.h"

#include <glacier/string/str_split.h>
#include <mammoth/util/debug.h>
#include <zcall.h>

glcr::ErrorOr<glcr::UniquePtr<VFSServer>> VFSServer::Create(
    Ext2Driver& driver) {
  z_cap_t endpoint_cap;
  RET_ERR(ZEndpointCreate(&endpoint_cap));
  return glcr::UniquePtr<VFSServer>(new VFSServer(endpoint_cap, driver));
}

glcr::ErrorCode VFSServer::HandleOpenFile(const OpenFileRequest& request,
                                          OpenFileResponse& response) {
  auto path_tokens = glcr::StrSplit(request.path(), '/');
  // Require all paths to be absolute rather than relative.
  // If the path starts with '/' then the first token will be empty.
  if (path_tokens.at(0) != "") {
    return glcr::INVALID_ARGUMENT;
  }

  ASSIGN_OR_RETURN(auto files, driver_.ReadDirectory(2));
  for (uint64_t i = 1; i < path_tokens.size() - 1; i++) {
    bool found_token = false;
    for (uint64_t j = 0; j < files.size() && !found_token; j++) {
      if (path_tokens.at(i) ==
          glcr::StringView(files.at(j).name, files.at(j).name_len)) {
        ASSIGN_OR_RETURN(files, driver_.ReadDirectory(files.at(j).inode));
        found_token = true;
      }
    }
    if (!found_token) {
      dbgln("Directory '{}' not found.",
            glcr::String(path_tokens.at(i)).cstr());
      return glcr::NOT_FOUND;
    }
  }

  uint64_t inode_num;
  mmth::OwnedMemoryRegion region;
  for (uint64_t j = 0; j < files.size(); j++) {
    if (path_tokens.at(path_tokens.size() - 1) ==
        glcr::StringView(files.at(j).name, files.at(j).name_len)) {
      inode_num = files.at(j).inode;
      ASSIGN_OR_RETURN(region, driver_.ReadFile(files.at(j).inode));
      break;
    }
  }
  if (!region) {
    dbgln("File '{}' not found.",
          glcr::String(path_tokens.at(path_tokens.size() - 1)).cstr());
  }

  response.set_path(request.path());
  // FIXME: There isn't really a reason we need to map the file into memory then
  // duplicate the cap. In the future just get the cap from the read then pass
  // it to the caller directly.
  response.set_memory(region.DuplicateCap());
  // TODO: Consider folding this up into the actual read call.
  ASSIGN_OR_RETURN(Inode * inode, driver_.GetInode(inode_num));
  // FIXME: This technically only sets the lower 32 bits.
  response.set_size(inode->size);
  return glcr::OK;
}

glcr::ErrorCode VFSServer::HandleGetDirectory(
    const GetDirectoryRequest& request, Directory& response) {
  auto path_tokens = glcr::StrSplit(request.path(), '/');

  if (path_tokens.at(0) != "") {
    return glcr::INVALID_ARGUMENT;
  }

  // If there is a trailing slash we can get rid of the empty string.
  if (path_tokens.at(path_tokens.size() - 1) == "") {
    path_tokens.PopBack();
  }

  ASSIGN_OR_RETURN(auto files, driver_.ReadDirectory(2));
  for (uint64_t i = 1; i < path_tokens.size(); i++) {
    bool found_token = false;
    for (uint64_t j = 0; j < files.size() && !found_token; j++) {
      if (path_tokens.at(i) ==
          glcr::StringView(files.at(j).name, files.at(j).name_len)) {
        ASSIGN_OR_RETURN(files, driver_.ReadDirectory(files.at(j).inode));
        found_token = true;
      }
    }
    if (!found_token) {
      dbgln("Directory '{}' not found.",
            glcr::String(path_tokens.at(i)).cstr());
      return glcr::NOT_FOUND;
    }
  }

  glcr::VariableStringBuilder filelist;
  for (uint64_t i = 0; i < files.size(); i++) {
    filelist.PushBack(glcr::StringView(files.at(i).name, files.at(i).name_len));
    filelist.PushBack(',');
  }
  // Remove trailing comma.
  if (filelist.size() > 0) {
    filelist.DeleteLast();
  }

  response.set_filenames(filelist.ToString());

  return glcr::OK;
}
