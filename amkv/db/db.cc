#include "db/db.h"

#include <bits/fs_fwd.h>
#include <fcntl.h>

#include <utility>

#include "comm/filename.h"
#include "comm/log.h"
#include "db/write_batch_internal.h"
#include "lsm/compaction.h"
#include "lsm/sstable.h"
#include "util/filename.h"

namespace amkv::db {

DB::DB(const comm::Options& options, std::string dbname)
    : env_(options.env), internal_comparator_(options.comparator), db_name_(std::move(dbname)) {
}

DB::~DB() {
  delete this->mem_;
  delete this->imm_;
}

comm::Status DB::NewDB() {
  version::VersionEdit edit;
  const std::string manifest = comm::DescriptorFileName(this->db_name_, 1);
  util::WritableFile* file;
  comm::Status status = this->env_->NewWritableFile(manifest, &file);
  if (comm::ErrorCode::kOk != status.Code()) {
    return status;
  }

  log::Writer log(file);
  std::string record;
  edit.EncodeTo(&record);
  status = log.AddRecord(record);
  if (comm::ErrorCode::kOk != status.Code()) {
    status = file->Sync();
  }
  if (status.Code() != comm::ErrorCode::kOk) {
    status = file->Close();
  }
  delete file;
  return status;
}

comm::Status DB::Open(const comm::Options& options, const std::string& name, DB** db) {
  DB* ptr = new DB(options, name);

  if (ptr->mem_ == nullptr) {
    ptr->mem_ = new table::MemTable(&ptr->internal_comparator_);
    ptr->mem_->Ref();

    std::uint64_t new_log_number = 0;
    ptr->env_->NewWritableFile(util::LogFileName(name, new_log_number), &ptr->log_file_);
    ptr->log_ = new log::Writer(ptr->log_file_);
  }

  *db = ptr;
  return comm::Status::OK();
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

  table::LookupKey lookup_key(key, 0);
  if (this->mem_->Get(lookup_key, value, &status)) {
  } else if (this->imm_ != nullptr && this->imm_->Get(lookup_key, value, &status)) {
  } else {
    table::SSTable sstable;
    std::uint64_t table_id = 0;
    std::string fname = util::TableFileName(this->db_name_, table_id);
    status = sstable.Get(fname, key, value);
  }

  return status;
}

comm::Status DB::Write(const comm::WriteOptions& options, WriteBatch* updates) {
  comm::Status status = comm::Status::OK();

  WriteBatchInternal write_batch_internal;
  write_batch_internal.SetSequence(0);
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

  status = makeRoomForWrite(true);
  if (status.Code() != comm::ErrorCode::kOk) {
    WARN("Make Room Failed");
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
      lsm::MinorCompaction minor(this->db_name_);
      if (minor.CanDoCompaction()) {
        this->imm_ = this->mem_;
        this->mem_ = new table::MemTable(&this->internal_comparator_);
        this->mem_->Ref();
        // it should be placed on background task
        comm::Status status = minor.Do(this->imm_);
        this->imm_ = nullptr;
      }
      break;
    }
  }

  return comm::Status::OK();
}

}  // namespace amkv::db