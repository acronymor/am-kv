#include "lsm/memtable.h"

#include <cassert>
#include <cstring>

#include "fmt/base.h"
#include "lsm/codec.h"
#include "lsm/skiplist_iter.h"
#include "util/codec.h"

namespace amkv::table {

LookupKey::LookupKey(const std::string_view user_key, lsm::SequenceNumber seq) {
  std::size_t len = EncodeMemTable(user_key, "", seq, lsm::ValueType::kTypeValue, &this->memtable_key_);
  this->start_ = 0;
  this->end_ = len;
  this->kstart_ = len - sizeof(std::uint64_t);
}

std::string_view LookupKey::user_key() const {
  return this->memtable_key_.substr(this->kstart_, this->end_ - sizeof(std::uint64_t) + 1);
};

std::string_view LookupKey::memtable_key() const { return this->memtable_key_; }

std::string_view LookupKey::internal_key() const {
  return this->memtable_key_.substr(this->kstart_, this->end_ - this->kstart_ + 1);
}

MemTable::MemTable(const lsm::InternalKeyComparator* comparator)
    : comparator_(new lsm::MemTableKeyComparator(comparator)), refs_(0),
      table_(new lsm::SkipList<const std::string_view, const std::string_view>(&arena_, comparator)){};

MemTable::~MemTable() {
  delete comparator_;
  delete table_;
}

void MemTable::Ref() { this->refs_++; }

void MemTable::UnRef() {
  this->refs_--;
  if (this->refs_ == 0) delete this;
}

void MemTable::Add(lsm::SequenceNumber seq, lsm::ValueType type, const std::string_view key,
                   const std::string_view value) {
  std::string out;
  std::size_t len = EncodeMemTable(key, value, seq, type, &out);
  char* buf = this->arena_.AllocateAligned(len);
  std::memcpy(buf, out.data(), out.size());
  std::string_view slice(buf, len);
  this->table_->Insert(slice);
}

bool MemTable::Get(const LookupKey& key, std::string* value, comm::Status* status) {
  std::string_view mem_key = key.memtable_key();
  lsm::SkipListIterator iter(this->table_);
  iter.Seek(mem_key);
  if (iter.Valid()) {
    const std::string_view entry = iter.Key();
    std::string real_key;

    std::uint64_t seq = 0;
    lsm::ValueType type = lsm::ValueType::kTypeDeletion;
    std::size_t len = DecodeMemTable(entry, &real_key, value, &seq, &type);

    return len == entry.size();
  }

  return false;
}

std::size_t MemTable::Usage() const { return this->arena_.Usage(); }
}  // namespace amkv::table