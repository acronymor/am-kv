#include <string_view>

#include "lsm/log_format.h"
#include "comm/status.h"
#include "util/writable_file.h"

namespace amkv::log {
class Writer {
 public:
  explicit Writer(util::WritableFile* dest);
  ~Writer() = default;

  Writer(const Writer&) = delete;
  Writer& operator=(const Writer&) = delete;

  comm::Status AddRecord(const std::string_view& slice);

 private:
  util::WritableFile* dest_;
};
}  // namespace amkv::log