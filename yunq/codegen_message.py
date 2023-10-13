
from parser import *

HEADER_PRELUDE = """
// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/string/string.h>

"""

MESSAGE_CLASS_PREFIX = """
class {name} {{
 public:
  {name}() {{}}
  // Delete copy and move until implemented.
  {name}(const {name}&) = delete;
  {name}({name}&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&); 
  glcr::ByteBuffer SerializeToBytes();
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
}
"""

type_to_str = {
        Type.U64: "uint64_t",
        Type.I64: "int64_t",
        Type.STRING: "glcr::String",
        Type.CAPABILITY: "zcap_t",
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
}}

}}  // namespace
"""

IMPL_PARSE_DEF = """
void {name}::ParseFromBytes(const glcr::ByteBuffer& bytes) {{
  CheckHeader();
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

IMPL_PARSE_DEF_END = """
}
"""

def _generate_message_class_impl(message: Message) -> str:
    impl = IMPL_PARSE_DEF.format(name=message.name) 

    for offset, field in enumerate(message.fields):
        if field.type == Type.U64:
            impl += IMPL_PARSE_U64.format(name = field.name, offset = offset)
        elif field.type == Type.I64:
            impl += IMPL_PARSE_I64.format(name = field.name, offset = offset)
        elif field.type == Type.STRING:
            impl += IMPL_PARSE_STRING.format(name = field.name, offset = offset);
        else:
            impl += "\n{} unimplemented\n".format(field.type.name)

    impl += IMPL_PARSE_DEF_END

    return impl


def generate_message_impl(file: str, ast: list[Decl]) -> str:
    impl = IMPL_PRELUDE.format(file=file)


    for decl in ast:
        if type(decl) != Message:
            continue
        impl += _generate_message_class_impl(decl)

    return impl
