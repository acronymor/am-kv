#include "lsm/log_writer.h"

namespace amkv::lsm {
namespace log {
Writer::Writer(util::WritableFile* dest) : dest_(dest) {}

util::Status Writer::AddRecord(const std::string_view& slice) {
  util::Status status = this->dest_->Append(slice);

  if (util::ErrorCode::kOk == status.Code()) {
    status = this->dest_->Flush();
  }

  return status;
}
}  // namespace log
}  // namespace amkv::lsm