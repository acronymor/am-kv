#include <iostream>
#include <string_view>

#include "comm/status.h"
#include "lsm/memtable.h"
#include "version_edit.h"

namespace amkv::table {
class SSTable {
 public:
  comm::Status Put(const std::string& fname, const table::MemTable* const memtable, version::FileMetaData* meta);

  comm::Status Get(const std::string& fname, const std::string_view key, std::string* out);
};
}  // namespace amkv::table
