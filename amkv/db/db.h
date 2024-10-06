#include "comm/options.h"
#include "comm/status.h"
#include "db/writer.h"
#include "lsm/log_writer.h"
#include "lsm/memtable.h"

namespace amkv::db {

class DB {
 public:
  DB(const comm::Options& options, std::string name);
  DB(const DB&) = delete;
  ~DB();

  static comm::Status Open(const comm::Options& options, const std::string& name, DB** db);

  comm::Status Delete(const comm::WriteOptions& options, const std::string_view key);

  comm::Status Put(const comm::WriteOptions& options, const std::string_view key, const std::string_view value);

  comm::Status Get(const comm::ReadOptions& options, const std::string_view key, std::string* value);

  comm::Status Write(const comm::WriteOptions& options, WriteBatch* updates);

 private:
  comm::Status makeRoomForWrite(bool force);

 private:
  std::string db_name_;

  table::MemTable* mem_{nullptr};
  table::MemTable* imm_{nullptr};

  const lsm::InternalKeyComparator internal_comparator_;
};
}  // namespace amkv::db
