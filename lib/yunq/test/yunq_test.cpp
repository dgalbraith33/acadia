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

TEST_CASE("Types Setter/Getter", "[yunq]") {
  ex::Types t;
  t.set_unsigned_int(1);
  t.set_signed_int(-1);
  t.set_str("test");

  REQUIRE(t.unsigned_int() == 1);
  REQUIRE(t.signed_int() == -1);
  REQUIRE(t.str() == "test");
}

TEST_CASE("Types Serialization", "[yunq]") {
  ex::Types a;
  a.set_unsigned_int(1);
  a.set_signed_int(-1);
  a.set_str("test");

  glcr::ByteBuffer buf(1024);
  a.SerializeToBytes(buf, 0);

  ex::Types b;
  yunq::MessageView v(buf, 0);
  REQUIRE(b.ParseFromBytes(v).ok() == true);

  REQUIRE(b.unsigned_int() == 1);
  REQUIRE(b.signed_int() == -1);
  REQUIRE(b.str() == "test");
}

TEST_CASE("Cap Setter/Getter", "[yunq]") {
  ex::Cap c;
  c.set_cap(1234);

  REQUIRE(c.cap() == 1234);
}

TEST_CASE("Cap Serialization Inline", "[yunq]") {
  ex::Cap a;
  a.set_cap(1234);

  glcr::ByteBuffer buf(1024);
  a.SerializeToBytes(buf, 0);

  ex::Cap b;
  yunq::MessageView v(buf, 0);
  REQUIRE(b.ParseFromBytes(v).ok() == true);

  REQUIRE(b.cap() == 1234);
}

TEST_CASE("Cap Serialization Sidebuffer", "[yunq]") {
  ex::Cap a;
  a.set_cap(1234);

  glcr::ByteBuffer buf(1024);
  glcr::CapBuffer caps(1);
  a.SerializeToBytes(buf, 0, caps);

  ex::Cap b;
  yunq::MessageView v(buf, 0);
  REQUIRE(b.ParseFromBytes(v, caps).ok() == true);

  REQUIRE(b.cap() == 1234);
}

TEST_CASE("Repeated Setter/Getter", "[yunq]") {
  ex::Repeated r;
  r.mutable_unsigned_ints().PushBack(1);
  r.add_unsigned_ints(2);
  uint64_t c = 3;
  r.add_unsigned_ints(glcr::Move(c));

  REQUIRE(r.unsigned_ints()[0] == 1);
  REQUIRE(r.unsigned_ints()[1] == 2);
  REQUIRE(r.unsigned_ints()[2] == 3);
}
