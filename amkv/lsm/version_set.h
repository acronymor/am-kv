
#include <comm/env.h>

#include <set>
#include <string>

#include "comm/options.h"
#include "lsm/db_format.h"
#include "lsm/log_writer.h"
#include "lsm/version.h"

namespace amkv::version {
class VersionSet {
  class Builder;

 public:
  VersionSet(const std::string& dbname, const comm::Options* options);
  ~VersionSet();

  VersionSet(const VersionSet&) = delete;
  VersionSet& operator=(const VersionSet&) = delete;

  comm::Status LogAndApply(VersionEdit* edit);
  void Finalize(Version* version);

  void MarkFileNumberUsed(std::uint64_t number) {
    if (next_file_number_ <= number) {
      next_file_number_ = number + 1;
    }
  }
  uint64_t LogNumber() const { return log_number_; }
  uint64_t PrevLogNumber() const { return prev_log_number_; }
  lsm::SequenceNumber ManifestFileNumber() const { return manifest_file_number_; }
  lsm::SequenceNumber NewFileNumber() { return next_file_number_++; }
  int NumLevelFiles(int level) const { return current_->files_[level].size(); }
  int64_t NumLevelBytes(int level) const {
    const std::vector<FileMetaData*>& files = current_->files_[level];
    int64_t sum = 0;
    for (size_t i = 0; i < files.size(); i++) {
      sum += files[i]->file_size;
    }
    return sum;
  }
  lsm::SequenceNumber LastSequence() const { return last_sequence_; }
  void SetLastSequence(lsm::SequenceNumber s) { last_sequence_ = s; }

  comm::Status Recover(bool* save_manifest);
  Version* Current() const;

 private:
  void appendVersion(Version* v);
  comm::Status writeSnapshot(log::Writer* log);

 private:
  lsm::SequenceNumber next_file_number_;
  lsm::SequenceNumber last_sequence_;
  lsm::SequenceNumber manifest_file_number_;
  lsm::SequenceNumber log_number_;
  lsm::SequenceNumber prev_log_number_;

  const comm::Options* options_{nullptr};
  const std::string dbname_;

  Version dummy_versions_;
  Version* current_;

  util::WritableFile* descriptor_file_;
  log::Writer* descriptor_log_;
};

class VersionSet::Builder {
  struct BySmallestKey {
    bool operator()(const FileMetaData* lhs, const FileMetaData* rhs) const { return true; }
  };

  struct LevelState {
    std::set<uint64_t> deleted_files;
    std::set<FileMetaData*, BySmallestKey>* added_files;
  };

 public:
  Builder(Version* base);
  ~Builder();

  void Apply(const VersionEdit* edit);
  void SaveTo(Version* version);
  void MaybeAddFile(Version* version, std::size_t level, FileMetaData* f);

 private:
  Version* base_;
  LevelState levels_[lsm::kNumLevels];
};
}  // namespace amkv::version