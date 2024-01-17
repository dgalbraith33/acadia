#include <catch2/catch_test_macros.hpp>

#include "example/example.yunq.h"

TEST_CASE("Basic Setter/Getter", "[yunq]") {
  ex::Basic b;
  b.set_field(1);
  REQUIRE(b.field() == 1);
}

TEST_CASE("Basic serialization", "[yunq]") {
  ex::Basic a;
  a.set_field(1);

  glcr::ByteBuffer buf(1024);
  a.SerializeToBytes(buf, 0);

  ex::Basic b;
  yunq::MessageView v(buf, 0);
  REQUIRE(b.ParseFromBytes(v).ok() == true);

  REQUIRE(b.field() == 1);
}
