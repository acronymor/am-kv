#include "db/db.h"

#include "fmt/base.h"
#include "gtest/gtest.h"

namespace amkv::db {
class DBTest : public testing::Test {};

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