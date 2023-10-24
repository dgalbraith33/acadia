
from parser import *

HEADER_PRELUDE = """
// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/string/string.h>
#include <ztypes.h>

"""

MESSAGE_CLASS_PREFIX = """
class {name} {{
 public:
  {name}() {{}}
  // Delete copy and move until implemented.
  {name}(const {name}&) = delete;
  {name}({name}&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&); 
  void ParseFromBytes(const glcr::ByteBuffer&, const glcr::CapBuffer&);
  void SerializeToBytes(glcr::ByteBuffer&);
  void SerializeToBytes(glcr::ByteBuffer&, glcr::CapBuffer&);
"""

MESSAGE_CLASS_SET_GET = """
  {type} {name}() {{ return {name}_; }}
  void set_{name}({type} value) {{
    {name}_ = value;
  }}
"""

MESSAGE_CLASS_PRIVATE = """
 private:
"""

MESSAGE_CLASS_FIELD = """
  {type} {name}_;
"""

MESSAGE_CLASS_SUFFIX = """
};
"""

type_to_str = {
        Type.U64: "uint64_t",
        Type.I64: "int64_t",
        Type.STRING: "glcr::String",
        Type.CAPABILITY: "z_cap_t",
        Type.BYTES: "glcr::Vector<uint8_t>"
        }

def _type_str(field_type: Type) -> str:
    return type_to_str[field_type]


def _generate_message_class(message: Message) -> str:
    class_decl = MESSAGE_CLASS_PREFIX.format(name = message.name)

    for field in message.fields:
        class_decl += MESSAGE_CLASS_SET_GET.format(name = field.name, type = _type_str(field.type))

    class_decl += MESSAGE_CLASS_PRIVATE

    for field in message.fields:
        class_decl += MESSAGE_CLASS_FIELD.format(name = field.name, type = _type_str(field.type))
    
    class_decl += MESSAGE_CLASS_SUFFIX
    return class_decl


def generate_message_header(ast: list[Decl]) -> str:
    header = HEADER_PRELUDE

    for decl in ast:
        if type(decl) != Message:
            continue
        header += _generate_message_class(decl)

    return header

IMPL_PRELUDE = """
#include "{file}.h"

namespace {{

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {{
  uint32_t offset;
  uint32_t length;
}};

void CheckHeader(const glcr::ByteBuffer& bytes) {{
  // TODO: Check ident.
  // TODO: Parse core size.
  // TODO: Parse extension size.
  // TODO: Check CRC32
  // TODO: Parse options.
}}

void WriteHeader(glcr::ByteBuffer& bytes, uint32_t core_size, uint32_t extension_size) {{
  bytes.WriteAt<uint32_t>(0, 0xDEADBEEF);  // TODO: Chose a more unique ident sequence.
  bytes.WriteAt<uint32_t>(4, core_size);
  bytes.WriteAt<uint32_t>(8, extension_size);
  bytes.WriteAt<uint32_t>(12, 0); // TODO: Calculate CRC32.
  bytes.WriteAt<uint64_t>(16, 0); // TODO: Add options.
}}

}}  // namespace
"""

IMPL_PARSE_DEF = """
void {name}::ParseFromBytes(const glcr::ByteBuffer& bytes) {{
  CheckHeader(bytes);
"""

IMPL_PARSE_DEF_CAP = """
void {name}::ParseFromBytes(const glcr::ByteBuffer& bytes, const glcr::CapBuffer& caps) {{
  CheckHeader(bytes);
"""

IMPL_PARSE_U64 = """
  set_{name}(bytes.At<uint64_t>(header_size + (8 * {offset})));
"""

IMPL_PARSE_I64 = """
  set_{name}(bytes.At<int64_t>(header_size + (8 * {offset})));
"""

IMPL_PARSE_STRING = """
  auto {name}_pointer = bytes.At<ExtPointer>(header_size + (8 * {offset}));

  set_{name}(bytes.StringAt({name}_pointer.offset, {name}_pointer.length));
"""

IMPL_SET_CAP_EMPTY = """
  set_{name}(0);
"""

IMPL_PARSE_CAP = """
  uint64_t {name}_ptr = bytes.At<uint64_t>(header_size + (8 * {offset}));

  set_{name}(caps.At({name}_ptr));
"""

IMPL_PARSE_DEF_END = """
}
"""

def _generate_message_parse_impl(message: Message) -> str:
    impl = "" 

    for with_cap in (False, True):
        impl += IMPL_PARSE_DEF.format(name=message.name) if not with_cap else IMPL_PARSE_DEF_CAP.format(name=message.name) 
        for offset, field in enumerate(message.fields):
            if field.type == Type.U64:
                impl += IMPL_PARSE_U64.format(name = field.name, offset = offset)
            elif field.type == Type.I64:
                impl += IMPL_PARSE_I64.format(name = field.name, offset = offset)
            elif field.type == Type.STRING:
                impl += IMPL_PARSE_STRING.format(name = field.name, offset = offset);
            elif field.type == Type.CAPABILITY:
                if with_cap:
                    impl += IMPL_PARSE_CAP.format(name = field.name, offset = offset)
                else:
                    impl += IMPL_SET_CAP_EMPTY.format(name = field.name)
            else:
                impl += "\n{} unimplemented\n".format(field.type.name)

        impl += IMPL_PARSE_DEF_END

    return impl

IMPL_SER_DEF = """
void {name}::SerializeToBytes(glcr::ByteBuffer& bytes) {{

  uint32_t next_extension = header_size + 8 * {num_fields};
  const uint32_t core_size = next_extension;
"""

IMPL_SER_CAP_DEF = """
void {name}::SerializeToBytes(glcr::ByteBuffer& bytes, glcr::CapBuffer& caps) {{

  uint32_t next_extension = header_size + 8 * {num_fields};
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
"""

IMPL_SER_U64 = """
  bytes.WriteAt<uint64_t>(header_size + (8 * {offset}), {name}());
"""

IMPL_SER_I64 = """
  bytes.WriteAt<int64_t>(header_size + (8 * {offset}), {name}());
"""

IMPL_SER_STRING = """
  ExtPointer {name}_ptr{{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t){name}().length(),
  }};

  bytes.WriteStringAt(next_extension, {name}());
  next_extension += {name}_ptr.length;

  bytes.WriteAt<ExtPointer>(header_size + (8 * {offset}), {name}_ptr);
"""

IMPL_SER_CAP_EMPTY = """
  // FIXME: Warn or error on serialization
"""

IMPL_SER_CAP = """
  caps.WriteAt(next_cap, {name}());
  bytes.WriteAt<uint64_t>(header_size + (8 * {offset}), next_cap++);
"""

IMPL_SER_DEF_END = """
  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, core_size, next_extension);
}
"""
def _generate_message_serialize_impl(message: Message) -> str:
    impl = ""

    for with_cap in (False, True):
        if with_cap:
            impl += IMPL_SER_CAP_DEF.format(name = message.name, num_fields = len(message.fields))
        else:
            impl += IMPL_SER_DEF.format(name = message.name, num_fields = len(message.fields))

        for offset, field in enumerate(message.fields):
            if field.type == Type.U64:
                impl += IMPL_SER_U64.format(name = field.name, offset = offset)
            elif field.type == Type.I64:
                impl += IMPL_SER_I64.format(name = field.name, offset = offset)
            elif field.type == Type.STRING:
                impl += IMPL_SER_STRING.format(name = field.name, offset = offset)
            elif field.type == Type.CAPABILITY:
                if with_cap:
                    impl += IMPL_SER_CAP.format(name = field.name, offset = offset)
                else:
                    impl += IMPL_SER_CAP_EMPTY
            else:
                impl += "\n{} unimplemented\n".format(field.type.name)
        impl += IMPL_SER_DEF_END

    return impl

def _generate_message_class_impl(message: Message) -> str:
    return _generate_message_parse_impl(message) + _generate_message_serialize_impl(message)


def generate_message_impl(file: str, ast: list[Decl]) -> str:
    impl = IMPL_PRELUDE.format(file=file)

    for decl in ast:
        if type(decl) != Message:
            continue
        impl += _generate_message_class_impl(decl)

    return impl
