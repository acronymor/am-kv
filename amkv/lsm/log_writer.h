#include <string_view>

#include "comm/status.h"
#include "lsm/log_format.h"
#include "util/writable_file.h"

namespace amkv::log {
class Writer {
 public:
  explicit Writer(util::WritableFile* dest);
  ~Writer() = default;

  Writer(const Writer&) = delete;
  Writer& operator=(const Writer&) = delete;

  comm::Status AddRecord(const std::string_view record);

 private:
  comm::Status emitPhysicalRecord(RecordType type, const char* ptr, std::size_t length);

  util::WritableFile* dest_;
  std::size_t block_offset_;
};  // namespace log
}  // namespace amkv::lsm