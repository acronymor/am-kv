#include "lsm/sstable.h"

#include <fcntl.h>

#include "gtest/gtest.h"
#include "util/file.h"

namespace amkv::table {
class SSTableTest : public testing::Test {
 protected:
  std::string fname = "/tmp/sstable_test.ldb";
  SSTable table;
};

TEST_F(SSTableTest, Simple) {
  lsm::BytewiseComparator comparator1;
  lsm::InternalKeyComparator comparator2(&comparator1);
  MemTable mem_table(&comparator2);
  lsm::SequenceNumber seq = 1;
  mem_table.Add(seq, lsm::ValueType::kTypeValue, "key1", "value1");
  mem_table.Add(seq, lsm::ValueType::kTypeValue, "key2", "value2");

  comm::Status status = table.Put(this->fname, &mem_table);
  ASSERT_EQ(comm::ErrorCode::kOk, status.Code());

  std::string out;
  status = table.Get(fname, "key1", &out);
  ASSERT_EQ(comm::ErrorCode::kOk, status.Code());
}
}  // namespace amkv::table