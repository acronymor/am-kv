#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>

#include "comm/status.h"
#include "lsm/db_format.h"

namespace amkv::version {
struct FileMetaData {
  int refs = 0;
  std::uint64_t number = 0;
  std::uint64_t file_size = 0;
  std::string smallest;
  std::string largest;
};

class VersionEdit {
  friend class VersionSet;

 public:
  void EncodeTo(std::string* dst) const;
  comm::Status DecodeFrom(std::string_view src);

  void AddFile(std::size_t level, const FileMetaData* meta);
  void RemoveFile(std::size_t level, std::uint64_t file);

  std::string DebugString() const;

 private:
  std::vector<std::pair<std::size_t, FileMetaData>> new_files_;
  std::set<std::pair<std::size_t, std::uint64_t>> deleted_files_;

 public:
  void SetLogNumber(uint64_t num) {
    has_log_number_ = true;
    log_number_ = num;
  }
  void SetPrevLogNumber(lsm::SequenceNumber num) {
    has_prev_log_number_ = true;
    prev_log_number_ = num;
  }
  void SetNextFile(lsm::SequenceNumber num) {
    has_next_file_number_ = true;
    next_file_number_ = num;
  }
  void SetLastSequence(lsm::SequenceNumber seq) {
    has_last_sequence_ = true;
    last_sequence_ = seq;
  }

 private:
  lsm::SequenceNumber prev_log_number_{0};
  lsm::SequenceNumber log_number_{0};
  lsm::SequenceNumber next_file_number_{0};
  lsm::SequenceNumber last_sequence_{0};
  bool has_comparator_{false};
  bool has_log_number_{false};
  bool has_prev_log_number_{false};
  bool has_next_file_number_{false};
  bool has_last_sequence_{false};
};
}  // namespace amkv::version
