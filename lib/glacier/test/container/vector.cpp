#include "container/vector.h"

#include <catch2/catch_test_macros.hpp>

using namespace glcr;

TEST_CASE("Build Vector", "[vector]") {
  Vector<uint64_t> v;
  REQUIRE(v.size() == 0);
}
