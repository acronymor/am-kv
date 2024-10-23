#include "lsm/log_writer.h"

#include "comm/status.h"
#include "util/codec.h"

namespace amkv::log {
Writer::Writer(util::WritableFile* dest) : dest_(dest), block_offset_(0) {}

comm::Status Writer::AddRecord(const std::string_view record) {
  const char* ptr = record.data();
  std::size_t left = record.size();

  comm::Status status = comm::Status::OK();
  bool begin = true;
  do {
    const std::size_t leftover = kBlockSize - this->block_offset_;
    if (leftover < kHeaderSize) {
      if (leftover > 0) {
        static_assert(kHeaderSize == 7, "");
        dest_->Append(std::string_view("\x00\x00\x00\x00\x00\x00", leftover));
      }
      block_offset_ = 0;
    }

    const std::size_t avail = kBlockSize - this->block_offset_ - kHeaderSize;
    const std::size_t fragment_length = left < avail ? left : avail;
    const bool end = left == fragment_length;

    RecordType type;
    if (begin && end) {
      type = RecordType::kFullType;
    } else if (begin) {
      type = RecordType::kFirstType;
    } else if (end) {
      type = RecordType::kLastType;
    } else {
      type = RecordType::kMiddleType;
    }

    status = this->emitPhysicalRecord(type, ptr, fragment_length);

    ptr += fragment_length;
    left -= fragment_length;
    begin = false;

  } while (status.Code() == comm::ErrorCode::kOk && left > 0);

  return status;
}  // namespace log

comm::Status Writer::emitPhysicalRecord(RecordType type, const char* ptr, std::size_t length) {
  std::uint32_t crc = 1;

  char buf[kHeaderSize];
  buf[0] = 0;
  buf[1] = 0;
  buf[2] = 0;
  buf[3] = 0;
  buf[4] = static_cast<char>(length & 0xff);
  buf[5] = static_cast<char>(length >> 8);
  buf[6] = static_cast<char>(type);

  comm::Status status = this->dest_->Append(std::string_view(buf, kHeaderSize));
  if (status.Code() == comm::ErrorCode::kOk) {
    status = this->dest_->Append(std::string_view(ptr, length));
    if (status.Code() == comm::ErrorCode::kOk) {
      status = this->dest_->Flush();
    }
  }
  this->block_offset_ += kHeaderSize + length;
  return status;
}
};  // namespace amkv::log