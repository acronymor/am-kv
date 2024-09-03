#include "gtest/gtest.h"

namespace amkv::util {
class ArenaTest : public testing::Test {
 protected:
  void SetUp() override{

  };
};

TEST_F(ArenaTest, Simple) { std::cout << "Hello Word" << std::endl; }
}  // namespace amkv::util