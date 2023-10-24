#include <stdint.h>
#include <stdlib.h>

[[nodiscard]] void* operator new(uint64_t size) { return malloc(size); }
[[nodiscard]] void* operator new[](uint64_t size) { return malloc(size); }

void operator delete(void*, uint64_t) {}
void operator delete[](void*) {}
void operator delete[](void*, uint64_t) {}
