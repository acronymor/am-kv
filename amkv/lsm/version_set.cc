#include "lsm/version_set.h"

#include <bits/fs_fwd.h>
#include <comm/filename.h>
#include <util/file.h>
#include <util/sequential_file.h>

#include <cassert>
#include <format>

#include "lsm/current.h"
#include "lsm/log_reader.h"
#include "util/writable_file.h"

namespace amkv::version {
VersionSet::VersionSet(const std::string& dbname, const comm::Options* options)
    : dbname_(dbname), options_(options), prev_log_number_(0), next_file_number_(2), last_sequence_(0),
      current_(nullptr) {
  this->appendVersion(new Version());
}

VersionSet::~VersionSet() {
  this->current_->Unref();
  delete this->descriptor_log_;
  delete this->descriptor_file_;
}

comm::Status VersionSet::LogAndApply(VersionEdit* edit) {
  if (!edit->has_log_number_) {
    edit->SetLogNumber(this->log_number_);
  }

  if (!edit->has_prev_log_number_) {
    edit->SetPrevLogNumber(this->prev_log_number_);
  }

  edit->SetNextFile(this->next_file_number_);
  edit->SetLastSequence(this->last_sequence_);

  comm::Status status = comm::Status::OK();
  Version* new_version = new Version();

  Builder builder(this->current_);
  builder.Apply(edit);
  builder.SaveTo(new_version);
  this->Finalize(new_version);

  const std::string new_manifest_file = comm::DescriptorFileName(this->dbname_, this->manifest_file_number_);
  if (this->descriptor_file_ == nullptr) {
    status = options_->env->NewWritableFile(new_manifest_file, &this->descriptor_file_);
    if (status.Code() == comm::ErrorCode::kOk) {
      this->descriptor_log_ = new log::Writer(this->descriptor_file_);
      status = this->writeSnapshot(this->descriptor_log_);
    }
  }

  if (status.Code() == comm::ErrorCode::kOk) {
    std::string record;
    edit->EncodeTo(&record);
    status = this->descriptor_log_->AddRecord(record);
    if (status.Code() == comm::ErrorCode::kOk) {
      status = this->descriptor_file_->Sync();
    }
  }

  if (status.Code() == comm::ErrorCode::kOk) {
    status = SetCurrentFile(this->options_->env, this->dbname_, this->manifest_file_number_);
  }

  if (status.Code() == comm::ErrorCode::kOk) {
    this->appendVersion(new_version);
    this->log_number_ = edit->log_number_;
    this->prev_log_number_ = edit->prev_log_number_;
  } else {
    delete new_version;
    if (!new_manifest_file.empty()) {
      delete descriptor_file_, descriptor_file_ = nullptr;
      delete descriptor_log_, descriptor_log_ = nullptr;
      status = util::DeleteFile(new_manifest_file);
    }
  }

  return status;
}

void VersionSet::Finalize(Version* version) {}

comm::Status VersionSet::Recover(bool* save_manifest) {
  std::string current;
  this->options_->env->ReadStringFromFileSync(comm::CurrentFileName(this->dbname_), &current);

  bool have_log_number = false;
  bool have_prev_log_number = false;
  bool have_next_file = false;
  bool have_last_sequence = false;
  std::uint64_t next_file = 0;
  std::uint64_t last_sequence = 0;
  std::uint64_t log_number = 0;
  std::uint64_t prev_log_number = 0;

  Builder builder(this->current_);

  util::SequentialFile* readable_log_file{nullptr};
  std::string filename = std::format("{}/{}", this->dbname_, current);
  comm::Status status = this->options_->env->NewSequentialFile(filename, &readable_log_file);
  log::Reader reader(readable_log_file);

  std::string record;
  std::string scratch;

  while (reader.ReadRecord(&record, &scratch)) {
    VersionEdit edit;
    status = edit.DecodeFrom(record);
    builder.Apply(&edit);

    if (edit.has_log_number_) {
      log_number = edit.log_number_;
      have_log_number = true;
    }

    if (edit.has_prev_log_number_) {
      prev_log_number = edit.prev_log_number_;
      have_prev_log_number = true;
    }

    if (edit.has_next_file_number_) {
      next_file = edit.next_file_number_;
      have_next_file = true;
    }

    if (edit.has_last_sequence_) {
      last_sequence = edit.last_sequence_;
      have_last_sequence = true;
    }
  }

  readable_log_file->Close();
  delete readable_log_file, readable_log_file = nullptr;

  if (status.Code() == comm::ErrorCode::kOk) {
    if (!have_next_file) {
      status = comm::Status(comm::ErrorCode::kFileError, "no meta-nextfile entry in descriptor");
    } else if (!have_log_number) {
      status = comm::Status(comm::ErrorCode::kFileError, "no meta-lognumber entry in descriptor");
    } else if (!have_last_sequence) {
      status = comm::Status(comm::ErrorCode::kFileError, "no last-sequence-number entry in descriptor");
    }
    if (!have_prev_log_number) {
      prev_log_number = 0;
    }

    this->MarkFileNumberUsed(prev_log_number);
    this->MarkFileNumberUsed(log_number);
  }

  if (status.Code() == comm::ErrorCode::kOk) {
    Version* new_version = new Version();
    builder.SaveTo(new_version);
    this->Finalize(new_version);
    this->appendVersion(new_version);

    this->manifest_file_number_ = next_file;
    this->next_file_number_ = next_file + 1;
    this->last_sequence_ = last_sequence;
    this->log_number_ = log_number;
    this->prev_log_number_ = prev_log_number;
  }

  *save_manifest = true;

  return status;
}

Version* VersionSet::Current() const { return this->current_; }

void VersionSet::appendVersion(Version* v) {
  if (this->current_ != nullptr) {
    this->current_->Unref();
  }
  this->current_ = v;
  v->Ref();

  v->prev_ = dummy_versions_.prev_;
  v->next_ = &dummy_versions_;
  v->prev_->next_ = v;
  v->next_->prev_ = v;
}
comm::Status VersionSet::writeSnapshot(log::Writer* log) {
  VersionEdit edit;

  for (int level = 0; level < lsm::kNumLevels; level++) {
    const std::vector<FileMetaData*>& files = current_->files_[level];
    for (size_t i = 0; i < files.size(); i++) {
      const FileMetaData* meta = files[i];
      edit.AddFile(level, meta);
    }
  }

  std::string record;
  edit.EncodeTo(&record);
  return log->AddRecord(record);
}

VersionSet::Builder::Builder(Version* base) : base_(base) {
  this->base_->Ref();
  BySmallestKey cmp;
  for (std::size_t level = 0; level < lsm::kNumLevels; level++) {
    this->levels_[level].added_files = new std::set<FileMetaData*, BySmallestKey>(cmp);
  }
}

VersionSet::Builder::~Builder() {}

void VersionSet::Builder::Apply(const VersionEdit* edit) {
  for (const auto& file : edit->deleted_files_) {
    const std::size_t level = file.first;
    const std::uint64_t number = file.second;
    this->levels_[level].deleted_files.insert(number);
  }

  for (std::size_t i = 0; i < edit->new_files_.size(); i++) {
    const std::size_t level = edit->new_files_[i].first;
    FileMetaData* meta = new FileMetaData(edit->new_files_[i].second);
    this->levels_[level].deleted_files.erase(meta->number);
    this->levels_[level].added_files->insert(meta);
  }
}

void VersionSet::Builder::SaveTo(Version* version) {
  for (std::size_t level = 0; level < lsm::kNumLevels; level++) {
    const std::vector<FileMetaData*>& base_files = this->base_->files_[level];
    std::vector<FileMetaData*>::const_iterator base_iter = base_files.begin();
    std::vector<FileMetaData*>::const_iterator base_end = base_files.end();

    std::set<FileMetaData*, BySmallestKey>* added_files = this->levels_[level].added_files;
    version->files_[level].reserve(base_files.size() + added_files->size());

    for (FileMetaData* add_file : *added_files) {
      for (; base_iter != base_end; base_iter++) {
        // cmp
        this->MaybeAddFile(version, level, *base_iter);
      }

      this->MaybeAddFile(version, level, add_file);
    }

    for (; base_iter != base_end; base_iter++) {
      this->MaybeAddFile(version, level, *base_iter);
    }
  }
}

void VersionSet::Builder::MaybeAddFile(Version* version, std::size_t level, FileMetaData* meta) {
  if (this->levels_[level].deleted_files.count(meta->number) == 0) {
    meta->refs++;
    version->files_[level].push_back(meta);
  }
}

}  // namespace amkv::version
