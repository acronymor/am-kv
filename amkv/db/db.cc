#include "db/db.h"

#include <bits/fs_fwd.h>
#include <fcntl.h>
#include <lsm/log_reader.h>
#include <util/dirname.h>

#include <utility>

#include "comm/filename.h"
#include "comm/log.h"
#include "db/write_batch_internal.h"
#include "fmt/chrono.h"
#include "lsm/compaction.h"
#include "lsm/current.h"
#include "lsm/sstable.h"
#include "util/filename.h"

namespace amkv::db {

DB::DB(const comm::Options& options, std::string dbname)
    : env_(options.env), internal_comparator_(options.comparator), db_name_(std::move(dbname)),
      versions_(new version::VersionSet(db_name_, &options)) {}

DB::~DB() {
  delete this->mem_, this->mem_ = nullptr;
  delete this->imm_, this->imm_ = nullptr;
  delete this->log_, this->log_ = nullptr;
  delete this->log_file_, this->log_file_ = nullptr;
}

comm::Status DB::NewDB() {
  version::VersionEdit edit;
  edit.SetLogNumber(0);
  edit.SetNextFile(2);
  edit.SetLastSequence(0);

  const std::string manifest = comm::DescriptorFileName(this->db_name_, 1);
  util::WritableFile* file;
  comm::Status status = this->env_->NewWritableFile(manifest, &file);
  if (comm::ErrorCode::kOk != status.Code()) {
    return status;
  }

  log::Writer manifest_writer(file);
  std::string record;
  edit.EncodeTo(&record);
  status = manifest_writer.AddRecord(record);
  if (comm::ErrorCode::kOk != status.Code()) {
    status = file->Sync();
  }
  if (status.Code() != comm::ErrorCode::kOk) {
    status = file->Close();
  }
  delete file;

  if (status.Code() == comm::ErrorCode::kOk) {
    status = version::SetCurrentFile(this->env_, this->db_name_, 1);
  }

  return status;
}

comm::Status DB::Open(const comm::Options& options, const std::string& name, DB** db) {
  DB* ptr = new DB(options, name);

  version::VersionEdit edit;
  bool save_manifest;
  comm::Status status = ptr->recover(&edit, &save_manifest);

  if (comm::ErrorCode::kOk == status.Code() && ptr->mem_ == nullptr) {
    std::uint64_t new_log_number = ptr->versions_->NewFileNumber();
    status = ptr->env_->NewWritableFile(util::LogFileName(name, new_log_number), &ptr->log_file_);
    if (comm::ErrorCode::kOk == status.Code()) {
      edit.SetLogNumber(new_log_number);
      ptr->log_ = new log::Writer(ptr->log_file_);
      ptr->logfile_number_ = new_log_number;
      ptr->mem_ = new table::MemTable(&ptr->internal_comparator_);
      ptr->mem_->Ref();
    }
  }

  if (comm::ErrorCode::kOk == status.Code() && save_manifest) {
    status = ptr->versions_->LogAndApply(&edit);
  }

  if (comm::ErrorCode::kOk == status.Code()) {
    ptr->removeObsoleteFiles();
  }

  *db = ptr;
  return status;
}

comm::Status DB::Delete(const comm::WriteOptions& options, const std::string_view key) {
  WriteBatch batch;
  batch.Delete(key);
  comm::Status status = this->Write(options, &batch);
  return status;
}

comm::Status DB::Put(const comm::WriteOptions& options, const std::string_view key, const std::string_view value) {
  WriteBatch batch;
  batch.Put(key, value);
  comm::Status status = this->Write(options, &batch);
  return status;
}

comm::Status DB::Get(const comm::ReadOptions& options, const std::string_view key, std::string* value) {
  comm::Status status = comm::Status::OK();
  version::Version* version = this->versions_->Current();

  table::LookupKey lookup_key(key, 0);
  if (this->mem_->Get(lookup_key, value, &status)) {
  } else if (this->imm_ != nullptr && this->imm_->Get(lookup_key, value, &status)) {
  } else {
    table::SSTable sstable;
    version::FileMetaData meta;
    version->ForEachOverlapping(lookup_key.user_key(), key, &meta);
    // TODO
    std::uint64_t table_id = meta.number;
    std::string fname = util::TableFileName(this->db_name_, table_id);
    status = sstable.Get(fname, key, value);
  }

  return status;
}

comm::Status DB::Write(const comm::WriteOptions& options, WriteBatch* updates) {
  comm::Status status = comm::Status::OK();
  status = makeRoomForWrite(updates == nullptr);

  lsm::SequenceNumber last_sequence = this->versions_->LastSequence();

  if (status.Code() == comm::ErrorCode::kOk && updates != nullptr) {
    WriteBatchInternal write_batch_internal;
    write_batch_internal.SetSequence(last_sequence + 1);
    last_sequence += write_batch_internal.Count();
    status = this->log_->AddRecord(write_batch_internal.Contents(updates));
    if (status.Code() == comm::ErrorCode::kOk) {
      status = this->log_file_->Sync();
      if (status.Code() != comm::ErrorCode::kOk) {
        WARN("Sync Failed");
        return status;
      }
    }
    status = write_batch_internal.InsertInto(updates, this->mem_);
    if (status.Code() != comm::ErrorCode::kOk) {
      WARN("Write Batch Insert Failed");
      return status;
    }

    this->versions_->SetLastSequence(last_sequence);
  }

  return status;
}

comm::Status DB::makeRoomForWrite(bool force) {
  while (true) {
    if (!force && this->mem_->Usage() <= 4 * 104 * 1024) {
      break;
    } else if (this->imm_ != nullptr) {
      INFO("Current memtable full; wating...");
    } else {
      this->imm_ = this->mem_;
      this->mem_ = new table::MemTable(&this->internal_comparator_);
      this->mem_->Ref();
      // TODO it should be placed on background task
      // TODO edit should be pick from versions
      this->logfile_number_ = this->versions_->NewFileNumber();
      version::Version* base = this->versions_->Current();
      base->Ref();

      lsm::MinorCompaction minor(this->db_name_);
      if (minor.CanDoCompaction()) {
        version::VersionEdit edit;

        comm::Status status = minor.Do(this->imm_, base, &edit, this->logfile_number_);
        base->Unref();

        if (status.Code() == comm::ErrorCode::kOk) {
          edit.SetPrevLogNumber(0);
          edit.SetLogNumber(this->logfile_number_);
          status = this->versions_->LogAndApply(&edit);
        }

        if (status.Code() == comm::ErrorCode::kOk) {
          this->imm_->UnRef();
          this->imm_ = nullptr;
          this->removeObsoleteFiles();
        }
      }
      break;
    }
  }

  return comm::Status::OK();
}

comm::Status DB::recover(version::VersionEdit* edit, bool* save_manifest) {
  comm::Status status = util::CreateDir(this->db_name_);

  if (!util::FileExists(comm::CurrentFileName(this->db_name_))) {
    status = NewDB();
    if (comm::ErrorCode::kOk != status.Code()) {
      return status;
    }
  }

  status = this->versions_->Recover(save_manifest);
  if (status.Code() != comm::ErrorCode::kOk) {
    return status;
  }

  std::vector<std::string> filenames = util::ListDir(this->db_name_);
  uint64_t number;
  util::FileType type;
  std::vector<std::uint64_t> logs;
  for (const auto& filename : filenames) {
    if (ParseFileName(filename, &number, &type)) {
      if (type == util::kLogFile) {
        logs.push_back(number);
      }
    }
  }

  table::MemTable* mem = nullptr;
  WriteBatch batch;

  std::sort(logs.begin(), logs.end());
  for (const auto& log : logs) {
    std::string filename = comm::LogFileName(this->db_name_, log);

    util::SequentialFile* log_file{nullptr};
    status = this->env_->NewSequentialFile(filename, &log_file);

    log::Reader reader(log_file);
    std::string scratch;
    std::string record;
    while (reader.ReadRecord(&record, &scratch)) {
      WriteBatchInternal write_batch_internal;
      write_batch_internal.SetContents(&batch, record);

      if (mem == nullptr) {
        mem = new table::MemTable(&this->internal_comparator_);
        mem->Ref();
      }

      write_batch_internal.InsertInto(&batch, mem);
    }
    delete log_file;

    this->versions_->MarkFileNumberUsed(log);
  }

  if (mem != nullptr) {
    *save_manifest = true;
    lsm::MinorCompaction minor(this->db_name_);
    status = minor.Do(mem, nullptr, edit, logs.size() - 1);
  }

  return status;
}

void DB::removeObsoleteFiles() {
  std::uint64_t number = 0;
  util::FileType type = util::kTempFile;
  std::vector<std::string> filenames = util::ListDir(this->db_name_);

  std::vector<std::string> files_to_delete;
  for (std::string& filename : filenames) {
    if (ParseFileName(filename, &number, &type)) {
      bool keep = true;
      switch (type) {
        case util::FileType::kLogFile:
          keep = ((number >= versions_->LogNumber()) || (number == versions_->PrevLogNumber()));
          break;
        case util::FileType::kDescriptorFile:
          keep = (number >= versions_->ManifestFileNumber());
          break;
        case util::FileType::kTableFile:
          // keep = (live.find(number) != live.end());
          keep = true;
          break;
        case util::FileType::kTempFile:
          // keep = (live.find(number) != live.end());
          break;
        case util::FileType::kCurrentFile:
        case util::FileType::kDBLockFile:
        case util::FileType::kInfoLogFile:
          keep = true;
          break;
      }

      if (!keep) {
        files_to_delete.emplace_back(filename);
        if (type == util::FileType::kTableFile) {
        }
      }
    }
  }

  for (const std::string& filename : files_to_delete) {
    util::DeleteFile(std::format("{0}/{1}", this->db_name_, filename));
  }
}
}  // namespace amkv::db