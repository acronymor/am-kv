#pragma once

#include <iostream>
#include <string_view>
#include <vector>

#include "lsm/db_format.h"
#include "lsm/version_edit.h"

namespace amkv::version {
class Version {
  friend class VersionSet;

 public:
  Version();
  ~Version();

  Version(const Version&) = delete;
  Version& operator=(const Version&) = delete;

  void Ref();
  void Unref();

  std::size_t PickLevelForMemTableOutput(std::string_view samllest, std::string_view largestest);
  void ForEachOverlapping(std::string_view user_key, std::string_view internal_key, FileMetaData* meta);
  std::string DebugString() const;

 private:
  Version* prev_{nullptr};
  Version* next_{nullptr};
  std::size_t refs_;

  std::vector<FileMetaData*> files_[lsm::kNumLevels];
  FileMetaData* file_to_compact_;
};
}  // namespace amkv::version