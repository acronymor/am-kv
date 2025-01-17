#include <comm/log.h>

#include <iostream>

#include "db/db.h"
#include "lsm/sstable.h"

namespace amkv {
std::uint32_t Init(int argc, char* argv[]) {
  if (comm::init_log(argv[0]) != 0) {
    FATAL("{}", "log init failed.")
    return -1;
  }

  db::DB* db;
  comm::Options options;
  std::string db_name = "t_db";
  comm::Status status = db::DB::Open(options, db_name, &db);

  comm::WriteOptions write_options;
  db->Put(write_options, "key1", "key1-value1");
  db->Write(write_options, nullptr);
  db->Put(write_options, "key2", "key2-value2");
  db->Write(write_options, nullptr);
  db->Put(write_options, "key3", "key3-value3");
  db->Write(write_options, nullptr);

  std::string value;
  comm::ReadOptions read_options;
  db->Get(read_options, "key2", &value);
  std::cout << "ok: output=>" <<  value << std::endl;

  return 0;
}
}  // namespace amkv

int main(int argc, char* argv[]) {
  std::uint32_t status = amkv::Init(argc, argv);
  std::cout << status << std::endl;
  return 0;
}
