#include "example.yunq.h"

namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

void CheckHeader(const glcr::ByteBuffer& bytes) {
  // TODO: Check ident.
  // TODO: Parse core size.
  // TODO: Parse extension size.
  // TODO: Check CRC32
  // TODO: Parse options.
}

void WriteHeader(glcr::ByteBuffer& bytes, uint64_t offset, uint32_t core_size, uint32_t extension_size) {
  bytes.WriteAt<uint32_t>(offset + 0, 0xDEADBEEF);  // TODO: Chose a more unique ident sequence.
  bytes.WriteAt<uint32_t>(offset + 4, core_size);
  bytes.WriteAt<uint32_t>(offset + 8, extension_size);
  bytes.WriteAt<uint32_t>(offset + 12, 0); // TODO: Calculate CRC32.
  bytes.WriteAt<uint64_t>(offset + 16, 0); // TODO: Add options.
}

}  // namespace




void OpenFileRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);



  auto {field.name}_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * {loop.index}));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));



  set_options(bytes.At<uint64_t>(offset + header_size + (8 * 2)));


}

void {message.name}::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);



  auto {field.name}_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * {loop.index}));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));



  set_options(bytes.At<uint64_t>(offset + header_size + (8 * 2)));


}




void File::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);



  auto {field.name}_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * {loop.index}));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));



  set_attrs(bytes.At<uint64_t>(offset + header_size + (8 * 2)));



  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_mem_cap(0);


}

void {message.name}::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);



  auto {field.name}_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * {loop.index}));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));



  set_attrs(bytes.At<uint64_t>(offset + header_size + (8 * 2)));



  uint64_t mem_cap_ptr = bytes.At<uint64_t>(offset + header_size + (8 * {offset}));

  set_mem_cap(caps.At(mem_cap_ptr));


}


