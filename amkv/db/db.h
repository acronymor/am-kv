#include "comm/options.h"
#include "comm/status.h"
#include "db/writer.h"
#include "lsm/log_writer.h"
#include "lsm/memtable.h"
#include "lsm/version_set.h"

namespace amkv::db {

class DB {
 public:
  DB(const comm::Options& options, std::string dbname);
  DB(const DB&) = delete;
  ~DB();

  comm::Status NewDB();

  static comm::Status Open(const comm::Options& options, const std::string& name, DB** db);

  comm::Status Delete(const comm::WriteOptions& options, const std::string_view key);

  comm::Status Put(const comm::WriteOptions& options, const std::string_view key, const std::string_view value);

  comm::Status Get(const comm::ReadOptions& options, const std::string_view key, std::string* value);

  comm::Status Write(const comm::WriteOptions& options, WriteBatch* updates);

 private:
  comm::Status makeRoomForWrite(bool force);
  comm::Status recover(version::VersionEdit* edit, bool* save_manifest);
  void removeObsoleteFiles();

 private:
  comm::Env* const env_;

  std::string db_name_;
  log::Writer* log_;
  util::WritableFile* log_file_;

  std::uint64_t logfile_number_;

  table::MemTable* mem_{nullptr};
  table::MemTable* imm_{nullptr};

  const lsm::InternalKeyComparator internal_comparator_;
  version::VersionSet* const versions_;
};
}  // namespace amkv::db
