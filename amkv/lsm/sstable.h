#include <cstdint>

#include "comm/status.h"
#include "lsm/memtable.h"

namespace amkv::table {
class SSTable {
 public:
  std::uint8_t Open();
  comm::Status BuildTable(const std::string& fname, const table::MemTable* const memtable);
  comm::Status FindTable(const std::string& fname);
};
}  // namespace amkv::table
