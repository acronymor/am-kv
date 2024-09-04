#include "util/arena.h"

#include <cstdint>

#include "gtest/gtest.h"

namespace amkv::util {
class ArenaTest : public testing::Test {};

struct Student {
  std::uint64_t id;
  std::string name;
  std::size_t age;
};

TEST_F(ArenaTest, Simple) {
  Arena arena;
  char* const memory = arena.AllocateAligned(sizeof(Student) * 2);
  Student* student = new (memory) Student();
  student->id = 1;
  student->name = "abc";
  student->age = 18;
  assert(static_cast<const void*>(student) == static_cast<const void*>(memory));
}
}  // namespace amkv::util