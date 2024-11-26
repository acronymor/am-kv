#include "lsm/sstable.h"

#include <fcntl.h>

#include "comm/options.h"
#include "gtest/gtest.h"
#include "lsm/sstable_builder.h"
#include "lsm/sstable_resolver.h"
#include "util/file.h"

namespace amkv::table {
class SSTableTest : public testing::Test {
 protected:
  std::string fname = "/tmp/sstable_test.ldb";
  SSTable table;
};

TEST_F(SSTableTest, Builder) {
  comm::Options options;
  util::WritableFile* file = new util::PosixWritableFile("/tmp/sstable_test.ldb");
  file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);

  SSTableBuilder builder(options, file);
  builder.Add("key1", "value1");
  builder.Add("key2", "value2");

  comm::Status status = builder.Finish();
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  status = file->Sync();
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  status = file->Close();
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  delete file;
}

TEST_F(SSTableTest, Resolver) {
  util::PosixAccessFile* file = new util::PosixAccessFile(this->fname);

  comm::Status status = file->Open(O_RDONLY | O_CLOEXEC);

  comm::Options options;

  SSTableResolver resolver(options, file);
  resolver.Finish();

  file->Close();
  delete file;
}

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
  status = table.Get(this->fname, "key1", &out);
  ASSERT_EQ("value1", out);
  status = table.Get(this->fname, "key2", &out);
  ASSERT_EQ("value2", out);
  status = table.Get(this->fname, "key1", &out);
  ASSERT_EQ(comm::ErrorCode::kOk, status.Code());
}
}  // namespace amkv::table