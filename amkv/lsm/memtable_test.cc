#include "lsm/memtable.h"

#include "comm/status.h"
#include "gtest/gtest.h"

namespace amkv::lsm {
class MemtableTest : public testing::Test {};

TEST_F(MemtableTest, Simple) {
  BytewiseComparator comparator1;
  InternalKeyComparator comparator2(&comparator1);

  table::MemTable mem_table(&comparator2);

  SequenceNumber seq = 1;
  mem_table.Add(seq, ValueType::kTypeValue, "key1", "value1");
  mem_table.Add(seq, ValueType::kTypeValue, "key2", "value2");

  table::LookupKey key1("key1", 1);
  std::string value1;
  comm::Status status = comm::Status::OK();
  mem_table.Get(key1, &value1, &status);
  EXPECT_EQ("value1", value1);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
}
}  // namespace amkv::lsm