#include "lsm/sstable.h"

namespace amkv::table {
std::uint8_t Table::Open() {
  std::cout << "DB::Open" << std::endl;
  return 0;
}
}  // namespace amkv::table