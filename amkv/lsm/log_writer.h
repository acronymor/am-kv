#include <string_view>

#include "util/status.h"
#include "util/writable_file.h"

namespace amkv::lsm {
namespace log {
class Writer {
 public:
  explicit Writer(util::WritableFile* dest);
  ~Writer() = default;

  Writer(const Writer&) = delete;
  Writer& operator=(const Writer&) = delete;

  util::Status AddRecord(const std::string_view& slice);

 private:
  util::WritableFile* dest_;
};
}  // namespace log
}  // namespace amkv::lsm