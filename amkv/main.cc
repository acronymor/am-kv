#include <iostream>

#include "db/db.h"
#include "table/table.h"

namespace amkv {
std::uint8_t Init() {
  db::DB db;
  db.Open();

  table::Table table;
  table.Open();
  return 0;
}
}  // namespace amkv

int main(int argc, char* argv[]) {
  std::uint8_t status = amkv::Init();
  std::cout << status << std::endl;
  return 0;
}
