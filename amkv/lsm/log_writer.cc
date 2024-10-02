#include "lsm/log_writer.h"

#include "comm/status.h"

#include "util/codec.h"

namespace amkv::log {
Writer::Writer(util::WritableFile* dest) : dest_(dest) {}

comm::Status Writer::AddRecord(const std::string_view& slice) {
  comm::Status status = this->dest_->Append(slice);

  if (comm::ErrorCode::kOk == status.Code()) {
    status = this->dest_->Flush();
  }

  return status;
}  // namespace log
}  // namespace amkv::lsm