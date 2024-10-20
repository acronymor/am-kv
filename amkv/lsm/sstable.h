#include <iostream>
#include <string_view>

#include "comm/status.h"
#include "lsm/memtable.h"

namespace amkv::table {
class SSTable {
 public:
  comm::Status Put(const std::string& fname, const table::MemTable* const memtable);

  comm::Status Get(const std::string& fname, const std::string_view key, std::string* out);
};
}  // namespace amkv::table
