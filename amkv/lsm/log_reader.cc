#include "lsm/log_reader.h"

#include <comm/log.h>

#include <iostream>

#include "log_format.h"

namespace amkv::log {
Reader::Reader(util::SequentialFile* dest)
    : dest_(dest), initial_offset_(0), last_record_offset_(0), end_of_buffer_offset_(0), checksum_(true),
      backing_store_(new char[kBlockSize]) {}

Reader::~Reader() { delete[] backing_store_; }

bool Reader::ReadRecord(std::string* scratch) {
  if (this->last_record_offset_ < this->initial_offset_) {
    ERROR("ERROR: ReadRecord");
  }
  std::uint64_t prospective_record_offset = 0;
  std::string fragment;
  while (true) {
    std::uint8_t record_type = this->readPhysicalRecord(&fragment);
    std::uint64_t physical_record_offset =
        this->end_of_buffer_offset_ - this->buffer_.size() - kHeaderSize - fragment.size();

    switch (record_type) {
      case kFullType: {
        prospective_record_offset = physical_record_offset;
        scratch->clear();
        scratch->assign(fragment.begin(), fragment.end());
        this->last_record_offset_ = prospective_record_offset;
      }
        return true;
      case kFirstType: {
        prospective_record_offset = physical_record_offset;
        scratch->assign(fragment.begin(), fragment.end());
      } break;
      case kMiddleType: {
        scratch->append(fragment.begin(), fragment.end());
      } break;
      case kLastType: {
        scratch->append(fragment.begin(), fragment.end());
        this->last_record_offset_ = prospective_record_offset;
        return true;
      } break;
      case kEof: {
        scratch->clear();
      }
        return false;
      case kBadRecord: {
        scratch->clear();
      } break;
      default: {
        scratch->clear();
      } break;
    }
  }

  return false;
}

std::uint8_t Reader::readPhysicalRecord(std::string* record) {
  while (true) {
    if (this->buffer_.size() < kHeaderSize) {
      if (!this->eof_) {
        this->buffer_ = "";
        comm::Status status = this->dest_->Read(kBlockSize, &buffer_, backing_store_);
        this->end_of_buffer_offset_ += this->buffer_.size();
        if (status.Code() != comm::ErrorCode::kOk) {
          this->buffer_ = "";
          this->eof_ = true;
          return kEof;
        } else if (this->buffer_.size() < kHeaderSize) {
          this->eof_ = true;
        }
        continue;
      } else {
        this->buffer_ = "";
        return kEof;
      }
    }

    const char* ptr = this->buffer_.data();
    const std::uint32_t a = static_cast<std::uint32_t>(ptr[4]) & 0xff;
    const std::uint32_t b = static_cast<std::uint32_t>(ptr[5]) & 0xff;
    const std::uint32_t length = a | (b << 8);
    RecordType type = static_cast<RecordType>(ptr[6]);

    if (kHeaderSize + length > this->buffer_.size()) {
      std::size_t drop_size = this->buffer_.size();
      this->buffer_ = "";

      if (!this->eof_) {
        return kBadRecord;
      }
      return kEof;
    }

    if (type == kZeroType && length == 0) {
      this->buffer_ = "";
      return kBadRecord;
    }

    if (this->checksum_) {
      // TODO
    }

    this->buffer_.remove_prefix(kHeaderSize + length);

    *record = std::string(ptr + kHeaderSize, length);
    return type;
  }
};

}  // namespace amkv::log
