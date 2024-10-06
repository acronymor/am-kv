#include "db/db.h"

#include <utility>

#include "db/write_batch_internal.h"
#include "lsm/compaction.h"

namespace amkv::db {

DB::DB(const comm::Options& options, std::string name)
    : db_name_(std::move(name)), internal_comparator_(options.comparator) {}

DB::~DB() {
  delete this->mem_;
  delete this->imm_;
}

comm::Status DB::Open(const comm::Options& options, const std::string& name, DB** db) {
  DB* ptr = new DB(options, name);

  if (ptr->mem_ == nullptr) {
    ptr->mem_ = new table::MemTable(&ptr->internal_comparator_);
    ptr->mem_->Ref();
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
    std::cout << "SSTable Get" << std::endl;
  }

  return status;
}

comm::Status DB::Write(const comm::WriteOptions& options, WriteBatch* updates) {
  comm::Status status = comm::Status::OK();
  WriteBatchInternal write_batch_internal;
  write_batch_internal.SetSequence(0);
  status = write_batch_internal.InsertInto(updates, this->mem_);

  status = makeRoomForWrite(true);

  return status;
}

comm::Status DB::makeRoomForWrite(bool force) {
  while (true) {
    if (!force && this->mem_->Usage() <= 4 * 104 * 1024) {
      break;
    } else if (this->imm_ != nullptr) {
      std::cout << "Current memtable full; wating...";
    } else {
      lsm::MinorCompaction minor;
      if (minor.CanDoCompaction()) {
        this->imm_ = this->mem_;
        this->mem_ = new table::MemTable(&this->internal_comparator_);
        this->mem_->Ref();
        // it should be placed on background task
        comm::Status status = minor.Do(this->imm_);
        // this->imm_ = nullptr;
      }
      break;
    }
  }

  return comm::Status::OK();
}
}  // namespace amkv::db