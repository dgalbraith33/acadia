#include "new"

#include <stdlib.h>

[[nodiscard]] void* operator new(std::size_t size) { return malloc(size); }
[[nodiscard]] void* operator new[](std::size_t size) { return malloc(size); }

void operator delete(void*, std::size_t) {}
