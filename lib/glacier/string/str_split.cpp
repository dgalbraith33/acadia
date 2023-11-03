#include "glacier/string/str_split.h"

namespace glcr {

Vector<StringView> StrSplit(const StringView& str, char delimiter) {
  Vector<StringView> strings;
  uint64_t cur_pos = 0;
  uint64_t next_pos = 0;
  while ((next_pos = str.find(delimiter, cur_pos)) != str.npos) {
    strings.PushBack(str.substr(cur_pos, next_pos - cur_pos));
    cur_pos = next_pos + 1;
  }
  strings.PushBack(str.substr(cur_pos, str.size() - cur_pos));
  return strings;
}

}  // namespace glcr
