#include "lsm/memtable_iter.h"

#include <comm/status.h>

#include "codec.h"

namespace amkv::table {
MemTableIterator::MemTableIterator(const MemTable* memtable) : iter_(new lsm::SkipListIterator(memtable->table_)) {}

MemTableIterator::~MemTableIterator() { delete iter_; }

bool MemTableIterator::Valid() const {
  return this->iter_->Valid();
}

void MemTableIterator::SeekToFirst() { return this->iter_->SeekToFirst(); }

void MemTableIterator::SeekToLast() { return this->iter_->SeekToLast(); }

void MemTableIterator::Seek(const std::string_view& target) {}

void MemTableIterator::Next() { this->iter_->Next(); }

void MemTableIterator::Prev() { this->iter_->Prev(); }

std::string_view MemTableIterator::Key() const {
  std::string key;
  std::string value;
  lsm::SequenceNumber seq;
  lsm::ValueType type;
  lsm::DecodeMemTable(this->iter_->Key(), &key, &value, &seq, &type);
  return {key.data(), key.size()};
}

std::string_view MemTableIterator::Value() const {
  std::string key;
  std::string value;
  lsm::SequenceNumber seq;
  lsm::ValueType type;
  lsm::DecodeMemTable(this->iter_->Key(), &key, &value, &seq, &type);
  return {value.data(), value.size()};
}

comm::Status MemTableIterator::Status() const { return comm::Status::OK(); }
}  // namespace amkv::table