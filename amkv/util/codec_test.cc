#include "util/codec.h"

#include "gtest/gtest.h"

namespace amkv::util {
class CodecTest : public testing::Test {};

TEST_F(CodecTest, EncodeUint8) {
  std::string out1;
  std::uint8_t in1 = 3;
  std::size_t len1 = EncodeUInt8(in1, &out1);
  EXPECT_EQ(sizeof(std::uint8_t), len1);

  std::string out2(out1);
  std::uint8_t in2 = 0;
  std::size_t len2 = DecodeUInt8(out2, &in2);
  EXPECT_EQ(sizeof(std::uint8_t), len2);
  EXPECT_EQ(in1, in2);
}

TEST_F(CodecTest, EncodeUint32) {
  std::string out1;
  std::uint32_t in1 = 99;
  std::size_t len1 = EncodeUInt32(in1, &out1);
  EXPECT_EQ(sizeof(std::uint32_t), len1);

  std::string out2(out1);
  std::uint32_t in2 = 0;
  std::size_t len2 = DecodeUInt32(out2, &in2);
  EXPECT_EQ(sizeof(std::uint32_t), len2);
  EXPECT_EQ(in1, in2);
}

TEST_F(CodecTest, EncodeUint64) {
  std::string out1;
  std::uint64_t in1 = 1999;
  std::size_t len1 = EncodeUInt64(in1, &out1);
  EXPECT_EQ(sizeof(std::uint64_t), len1);

  std::string out2(out1);
  std::uint64_t in2 = 0;
  std::size_t len2 = DecodeUInt64(out2, &in2);
  EXPECT_EQ(sizeof(std::uint64_t), len2);
  EXPECT_EQ(in1, in2);
}

TEST_F(CodecTest, EncodeBytes) {
  std::string in1 = "1234";
  std::string out1;
  std::size_t len = EncodeBytes(in1, &out1);

  out1.append(3, '5');
  std::string out2;
  len = DecodeBytes(out1, &out2);
  EXPECT_EQ("1234", out2);
}
}  // namespace amkv::util