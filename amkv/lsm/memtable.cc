#include "lsm/memtable.h"

namespace amkv::table {
MemTable::MemTable() : refs_(0) {}

void MemTable::Ref() { this->refs_++; }

void MemTable::UnRef() { this->refs_--; }

void MemTable::Add(db::SequenceNumber seq, db::ValueType type, const std::string_view& key,
                   const std::string_view& value) {
  // TODO
}
}  // namespace amkv::table