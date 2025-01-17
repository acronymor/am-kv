#include "lsm/version.h"
namespace amkv::version {

Version::Version() : refs_(0), next_(this), prev_(this) {}

Version::~Version() {}

void Version::Ref() { this->refs_++; }

void Version::Unref() {
  this->refs_--;
  if (this->refs_ == 0) {
    delete this;
  }
}

std::size_t Version::PickLevelForMemTableOutput(const std::string_view samllest, const std::string_view largest) {
  std::size_t level = 0;
  return level;
}

void Version::ForEachOverlapping(std::string_view user_key, std::string_view internal_key, FileMetaData* meta) {
  std::vector<FileMetaData*> tmp;

  for (std::size_t level = 0; level < lsm::kNumLevels; level++) {
    std::size_t files_num = this->files_[level].size();
    if (files_num == 0) {
      continue;
    }

    for (std::size_t file = 0; file < files_num; file++) {
      FileMetaData* f = this->files_[level][file];
      if (internal_key >= this->files_[level][file]->smallest && internal_key <= this->files_[level][file]->largest) {
        *meta = *f;
      }
    }
  }
}

std::string Version::DebugString() const {
  std::string s;
  for (std::size_t level = 0; level < lsm::kNumLevels; level++) {
    s.append("--level ");
    const std::vector<FileMetaData*>& files = this->files_[level];
    for (std::size_t i = 0; i < files.size(); i++) {
      s.append(" ");
      s.append(std::to_string(files[i]->number));
      s.append(":");
      s.append("[");
      s.append(files[i]->smallest);
      s.append("..");
      s.append(files[i]->largest);
      s.append("]");
    }
  }
}

}  // namespace amkv::version
