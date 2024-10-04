#include <comm/log.h>

#include <iostream>

#include "db/db.h"
#include "lsm/sstable.h"
#include "util/arena.h"

namespace amkv {
std::uint32_t Init(int argc, char* argv[]) {
  if (amdb::comm::init_log(argv[0]) != 0) {
    FATAL("{}", "log init failed.")
    return -1;
  }

  db::DB* db;
  comm::Options options;
  std::string db_name = "t_db";
  comm::Status status = db::DB::Open(options, db_name, &db);
  db->Put(comm::WriteOptions(), "key", "value");

  /*
  table::Table table;
  table.Open();

  util::Arena arena;
  */
  return 0;
}
}  // namespace amkv

int main(int argc, char* argv[]) {
  std::uint32_t status = amkv::Init(argc, argv);
  std::cout << status << std::endl;
  return 0;
}
