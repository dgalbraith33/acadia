#include "container/vector.h"

#include <catch2/catch_test_macros.hpp>

using namespace glcr;

TEST_CASE("Empty Vector", "[vector]") {
  Vector<uint64_t> v;
  REQUIRE(v.size() == 0);
  REQUIRE(v.empty());
}

TEST_CASE("Push/Pop Vector", "[vector]") {
  Vector<uint64_t> v;
  v.PushBack(42);
  REQUIRE(v.size() == 1);
  REQUIRE(v.capacity() >= 1);
  v.PushBack(33);
  REQUIRE(v.size() == 2);
  REQUIRE(v.capacity() >= 2);

  REQUIRE(v.at(0) == 42);
  REQUIRE(v[0] == 42);
  REQUIRE(v.at(1) == 33);
  REQUIRE(v[1] == 33);

  REQUIRE(v.PopBack() == 33);
  REQUIRE(v.size() == 1);
  REQUIRE(v.PopBack() == 42);
  REQUIRE(v.size() == 0);
}

class ConstructRecorder {
 public:
  static uint64_t construct_cnt;
  static uint64_t copy_cnt;
  static uint64_t move_cnt;
  ConstructRecorder() { construct_cnt++; }
  ConstructRecorder(const ConstructRecorder&) { copy_cnt++; }
  ConstructRecorder& operator=(const ConstructRecorder&) {
    copy_cnt++;
    return *this;
  }

  ConstructRecorder(ConstructRecorder&&) { move_cnt++; }
  ConstructRecorder& operator=(ConstructRecorder&&) {
    move_cnt++;
    return *this;
  }

  static void Reset() {
    construct_cnt = 0;
    copy_cnt = 0;
    move_cnt = 0;
  }

 private:
  uint64_t dummy_data = 0;
};

uint64_t ConstructRecorder::construct_cnt = 0;
uint64_t ConstructRecorder::copy_cnt = 0;
uint64_t ConstructRecorder::move_cnt = 0;

TEST_CASE("Data-Type Construction", "[vector]") {
  ConstructRecorder::Reset();
  Vector<ConstructRecorder> v;

  SECTION("Copy Insert") {
    ConstructRecorder obj;
    v.PushBack(obj);
    // This is overfitted on the implementation which also default constructs
    // the held objects when allocating a new backing array.
    REQUIRE(ConstructRecorder::construct_cnt == 2);
    REQUIRE(ConstructRecorder::copy_cnt == 1);
    REQUIRE(ConstructRecorder::move_cnt == 0);
  }

  SECTION("Move Insert") {
    ConstructRecorder obj;
    v.PushBack(glcr::Move(obj));
    // This is overfitted on the implementation which also default constructs
    // the held objects when allocating a new backing array.
    REQUIRE(ConstructRecorder::construct_cnt == 2);
    REQUIRE(ConstructRecorder::copy_cnt == 0);
    REQUIRE(ConstructRecorder::move_cnt == 1);
  }

  SECTION("RValue Insert") {
    v.PushBack({});
    // This is overfitted on the implementation which also default constructs
    // the held objects when allocating a new backing array.
    REQUIRE(ConstructRecorder::construct_cnt == 2);
    REQUIRE(ConstructRecorder::copy_cnt == 0);
    REQUIRE(ConstructRecorder::move_cnt == 1);
  }

  SECTION("Emplace Insert") {
    v.EmplaceBack();
    // This is overfitted on the implementation which also default constructs
    // the held objects when allocating a new backing array.
    REQUIRE(ConstructRecorder::construct_cnt == 2);
    REQUIRE(ConstructRecorder::copy_cnt == 0);
    REQUIRE(ConstructRecorder::move_cnt == 1);
  }

  SECTION("PopBack Move") {
    v.EmplaceBack();
    ConstructRecorder obj = v.PopBack();

    // This is overfitted on the implementation which also default constructs
    // the held objects when allocating a new backing array.
    REQUIRE(ConstructRecorder::construct_cnt == 2);
    REQUIRE(ConstructRecorder::copy_cnt == 0);
    // 1 from emplace, 1 from pop. (No additional regular constructions).
    REQUIRE(ConstructRecorder::move_cnt == 2);
  }
}

TEST_CASE("Vector Move", "[vector]") {
  ConstructRecorder::Reset();

  Vector<ConstructRecorder> v;
  v.PushBack({});
  v.PushBack({});
  v.PushBack({});

  uint64_t construct = ConstructRecorder::construct_cnt;
  uint64_t copy = ConstructRecorder::copy_cnt;
  uint64_t move = ConstructRecorder::move_cnt;

  Vector<ConstructRecorder> v2(glcr::Move(v));

  REQUIRE(v2.size() == 3);
  REQUIRE(v2.capacity() >= 3);
  REQUIRE(ConstructRecorder::construct_cnt == construct);
  REQUIRE(ConstructRecorder::copy_cnt == copy);
  REQUIRE(ConstructRecorder::move_cnt == move);

  Vector<ConstructRecorder> v3 = glcr::Move(v2);

  REQUIRE(v3.size() == 3);
  REQUIRE(v3.capacity() >= 3);
  REQUIRE(ConstructRecorder::construct_cnt == construct);
  REQUIRE(ConstructRecorder::copy_cnt == copy);
  REQUIRE(ConstructRecorder::move_cnt == move);
}

TEST_CASE("Vector Iterator", "[vector]") {
  Vector<uint64_t> v;
  for (uint64_t i = 0; i < 100; i++) {
    v.PushBack(42);
  }

  SECTION("For Range Loop") {
    uint64_t iters = 0;
    for (uint64_t i : v) {
      REQUIRE(i == 42);
      iters++;
    }
    REQUIRE(iters == 100);
  }

  SECTION("Raw Iter Loop") {
    uint64_t iters = 0;
    for (auto it = v.begin(); it != v.end(); ++it) {
      REQUIRE(*it == 42);
      iters++;
    }
    REQUIRE(iters == 100);
  }
}
