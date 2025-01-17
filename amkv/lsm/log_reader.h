#include "comm/status.h"
#include "lsm/log_format.h"
#include "util/sequential_file.h"

namespace amkv::log {
class Reader {
 public:
  explicit Reader(util::SequentialFile* dest);
  ~Reader();

  Reader(const Reader&) = delete;
  Reader& operator=(const Reader&) = delete;

  bool ReadRecord(std::string* record, std::string* scratch);

 private:
  enum EndRecordType {
    kEof = static_cast<std::uint8_t>(kMaxRecordType) + 1,
    kBadRecord = static_cast<std::uint8_t>(kMaxRecordType) + 2
  };

  std::uint8_t readPhysicalRecord(std::string* record);

  char* const backing_store_;
  std::string_view buffer_;
  bool eof_;

  bool const checksum_;

  util::SequentialFile* dest_;
  std::uint64_t const initial_offset_;

  std::uint64_t last_record_offset_;
  std::uint64_t end_of_buffer_offset_;
};
}  // namespace amkv::log