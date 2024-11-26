#include "db/db.h"

#include "db/write_batch_internal.h"
#include "fmt/base.h"
#include "gtest/gtest.h"
#include "lsm/compaction.h"

namespace amkv::db {
class DBTest : public testing::Test {};

TEST_F(DBTest, CompactionWrite) {
  WriteBatch batch;
  batch.Put("key1", "value1");
  batch.Put("key2", "value2");

  std::string db_name = "t_db";
  lsm::MinorCompaction minor(db_name);

  lsm::BytewiseComparator comparator1;
  lsm::InternalKeyComparator comparator2(&comparator1);
  table::MemTable mem_table(&comparator2);

  WriteBatchInternal write_batch_internal;
  comm::Status status = write_batch_internal.InsertInto(&batch, &mem_table);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  version::VersionEdit edit;
  version::Version base;
  status = minor.Do(&mem_table, &base, &edit);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
}

TEST_F(DBTest, Simple) {
  DB* db;
  comm::Options options;
  std::string db_name = "t_db";
  comm::Status status = DB::Open(options, db_name, &db);

  status = db->Put(comm::WriteOptions(), "key", "value");
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  std::string value;
  status = db->Get(comm::ReadOptions(), "key", &value);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  EXPECT_EQ("value", value);
}
}  // namespace amkv::db