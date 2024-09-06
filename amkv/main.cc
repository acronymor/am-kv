#include <util/log.h>

#include <iostream>

#include "db/db.h"
#include "table/table.h"

namespace amkv {
std::uint32_t Init(int argc, char* argv[]) {
  if (amdb::init_log(argv[0]) != 0) {
    FATAL("{}", "log init failed.")
    return -1;
  }

  table::Table table;
  table.Open();
  return 0;
}
}  // namespace amkv

int main(int argc, char* argv[]) {
  std::uint32_t status = amkv::Init(argc, argv);
  std::cout << status << std::endl;
  return 0;
}
