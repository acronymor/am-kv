#include "lsm/compaction.h"

#include "gtest/gtest.h"

namespace amkv::lsm {
class MinorCompactionTest : public testing::Test {};

TEST_F(MinorCompactionTest, Simple) {
  SequenceNumber seq = 1;
  const std::string db_name = "t_db";
  MinorCompaction minor(db_name);

  BytewiseComparator comparator1;
  InternalKeyComparator comparator2(&comparator1);
  table::MemTable mem_table(&comparator2);

  mem_table.Add(seq, ValueType::kTypeValue, "key1", "value1");
  mem_table.Add(seq, ValueType::kTypeValue, "key2", "value2");

  comm::Status status = minor.Do(&mem_table);

  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
}

}  // namespace amkv::lsm