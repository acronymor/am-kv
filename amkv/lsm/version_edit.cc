#include "lsm/version_edit.h"

#include <util/codec.h>

namespace amkv::version {

enum struct Tag : uint8_t {
  kComparator = 1,
  kLogNumber = 2,
  kNextFileNumber = 3,
  kLastSequence = 4,
  kCompactPointer = 5,
  kDeletedFile = 6,
  kNewFile = 7,
  kPrevLogNumber = 9
};

void VersionEdit::EncodeTo(std::string* dst) const {
  if (this->has_log_number_) {
    util::EncodeUInt8(static_cast<std::uint8_t>(Tag::kLogNumber), dst);
    util::EncodeUInt64(this->log_number_, dst);
  }
  if (this->has_prev_log_number_) {
    util::EncodeUInt8(static_cast<std::uint8_t>(Tag::kPrevLogNumber), dst);
    util::EncodeUInt64(this->prev_log_number_, dst);
  }
  if (this->has_next_file_number_) {
    util::EncodeUInt8(static_cast<std::uint8_t>(Tag::kNextFileNumber), dst);
    util::EncodeUInt64(this->next_file_number_, dst);
  }
  if (this->has_last_sequence_) {
    util::EncodeUInt8(static_cast<std::uint8_t>(Tag::kLastSequence), dst);
    util::EncodeUInt64(this->last_sequence_, dst);
  }

  for (const auto& file : this->deleted_files_) {
    util::EncodeUInt8(static_cast<std::uint8_t>(Tag::kDeletedFile), dst);
    util::EncodeUInt64(file.first, dst);
    util::EncodeUInt64(file.second, dst);
  }

  for (std::size_t i = 0; i < this->new_files_.size(); i++) {
    const FileMetaData& meta = this->new_files_[i].second;
    util::EncodeUInt8(static_cast<std::uint8_t>(Tag::kNewFile), dst);
    util::EncodeUInt64(this->new_files_[i].first, dst);
    util::EncodeUInt64(meta.number, dst);
    util::EncodeUInt64(meta.file_size, dst);
    util::EncodeBytes(meta.smallest, dst);
    util::EncodeBytes(meta.largest, dst);
  }
}

comm::Status VersionEdit::DecodeFrom(std::string_view src) {
  std::size_t len = 0;

  while (len != src.size()) {
    std::uint8_t raw_tag = 0;
    len += util::DecodeUInt8(src.substr(len, sizeof(raw_tag)), &raw_tag);
    switch (static_cast<Tag>(raw_tag)) {
      case Tag::kLogNumber: {
        len += util::DecodeUInt64(src.substr(len, sizeof(this->log_number_)), &this->log_number_);
        this->has_log_number_ = true;
      } break;
      case Tag::kPrevLogNumber: {
        len += util::DecodeUInt64(src.substr(len, sizeof(this->prev_log_number_)), &this->prev_log_number_);
        this->has_prev_log_number_ = true;
      } break;
      case Tag::kNextFileNumber: {
        len += util::DecodeUInt64(src.substr(len, sizeof(this->next_file_number_)), &this->next_file_number_);
        this->has_next_file_number_ = true;
      } break;
      case Tag::kLastSequence: {
        len += util::DecodeUInt64(src.substr(len, sizeof(this->last_sequence_)), &this->last_sequence_);
        this->has_last_sequence_ = true;
      } break;
      case Tag::kNewFile: {
        FileMetaData meta;

        std::uint64_t level = 0;
        len += util::DecodeUInt64(src.substr(len, sizeof(level)), &level);
        len += util::DecodeUInt64(src.substr(len, sizeof(meta.number)), &meta.number);
        len += util::DecodeUInt64(src.substr(len, sizeof(meta.file_size)), &meta.file_size);
        len += util::DecodeBytes(src.substr(len), &meta.smallest);
        len += util::DecodeBytes(src.substr(len), &meta.largest);
        this->new_files_.emplace_back(level, meta);
      }; break;
      case Tag::kDeletedFile: {
        std::uint64_t level = 0;
        len += util::DecodeUInt64(src.substr(len, sizeof(level)), &level);
        std::uint64_t number = 0;
        len += util::DecodeUInt64(src.substr(len, sizeof(number)), &number);
        this->deleted_files_.emplace(level, number);
      }; break;
      default:
        break;
    }
  }
  return comm::Status::OK();
}

void VersionEdit::AddFile(std::size_t level, const FileMetaData* meta) { this->new_files_.emplace_back(level, *meta); }

void VersionEdit::RemoveFile(std::size_t level, std::uint64_t file) {
  this->deleted_files_.insert(std::make_pair(level, file));
}

std::string VersionEdit::DebugString() const {
  std::string s;
  s.append("VersionEdit {");
  s.append("\n Comparator: ");
  s.append("\n LogNumber: ").append(std::to_string(this->log_number_));
  s.append("\n PreLogNumber: ").append(std::to_string(this->prev_log_number_));
  s.append("\n NextFile: ").append(std::to_string(this->next_file_number_));
  s.append("\n LastSeq: ").append(std::to_string(this->next_file_number_));
  for (const auto& file : this->deleted_files_) {
    s.append("\n RemovedFile: ").append(std::to_string(file.first)).append(" ").append(std::to_string(file.second));
  }
  for (const auto& file : this->new_files_) {
    const FileMetaData& meta = file.second;
    s.append("\n AddFile: ");
    s.append(std::to_string(file.first)).append(" ");
    s.append(std::to_string(meta.number)).append(" ");
    s.append(std::to_string(meta.file_size)).append(" ");
    s.append(meta.smallest).append("..").append(meta.largest);
  }
  s.append("\n}\n");
  return s;
}
}  // namespace amkv::version