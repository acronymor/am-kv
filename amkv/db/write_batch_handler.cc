#include "db/write_batch_handler.h"

namespace amkv::db {

WriteBatchHandler::WriteBatchHandler(table::MemTable* memtable) : memtable_(memtable) {}

void WriteBatchHandler::Put(const std::string_view key, const std::string_view value) {
  this->memtable_->Add(this->sequence_, lsm::ValueType::kTypeValue, key, value);
  this->sequence_++;
}

void WriteBatchHandler::Delete(const std::string_view key) {
  this->memtable_->Add(this->sequence_, lsm::ValueType::kTypeDeletion, key, "");
  this->sequence_++;
}

}  // namespace amkv::db