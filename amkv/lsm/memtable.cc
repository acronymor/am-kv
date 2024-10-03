#include "lsm/memtable.h"

#include <util/codec.h>

#include <cstring>

namespace amkv::table {

LookupKey::LookupKey(const std::string_view user_key, lsm::SequenceNumber seq) {
  std::size_t len = 0;
  this->start_ = len;

  len += util::EncodeBytes(user_key, &this->key_);
  this->kstart_ = len;

  len += util::EncodeUInt64(seq, &this->key_);
  this->end_ = this->kstart_ + len + sizeof(std::uint32_t);
}

std::string_view LookupKey::user_key() const { return {this->key_.data() + this->end_, sizeof(std::uint8_t)}; }

std::string_view LookupKey::memtable_key() const { return {this->key_.data() + start_, this->end_ - this->start_}; }

std::string_view LookupKey::internal_key() const { return {this->key_.data() + kstart_, end_ - kstart_}; }

MemTable::MemTable(const lsm::InternalKeyComparator& comparator)
    : comparator_(lsm::MemTableKeyComparator(comparator)), refs_(0), table_(&arena_, comparator_) {}

MemTable::~MemTable() { assert(refs_ == 0); }

void MemTable::Ref() { this->refs_++; }

void MemTable::UnRef() {
  this->refs_--;
  if (this->refs_ == 0) delete this;
}

void MemTable::Add(lsm::SequenceNumber seq, lsm::ValueType type, const std::string_view key,
                   const std::string_view value) {
  std::string out;
  std::size_t len = 0;
  len += util::EncodeBytes(key, &out);
  len += util::EncodeUInt64(seq, &out);
  len += util::EncodeUInt8(static_cast<std::uint8_t>(type), &out);
  len += util::EncodeBytes(value, &out);
  char* buf = this->arena_.AllocateAligned(len);
  std::memcpy(buf, out.data(), out.size());
  std::string_view slice(buf, len);
  this->table_.Insert(slice);
}

bool MemTable::Get(const LookupKey& key, std::string* value, comm::Status* status) {
  std::string_view mem_key = key.memtable_key();
  Table::Iterator iter(&this->table_);
  iter.Seek(mem_key);
  if (iter.Valid()) {
    std::string_view entry = iter.key();

    std::uint32_t len = 0;
    util::DecodeUInt32(entry.substr(len, sizeof(std::uint32_t)), &len);

    std::string key1;
    len = util::DecodeBytes(entry.substr(0, len + sizeof(std::uint32_t)), &key1);

    std::uint64_t seq = 0;
    len += util::DecodeUInt64(entry.substr(len, sizeof(std::uint64_t)), &seq);

    std::uint8_t type1 = 0;
    len += util::DecodeUInt8(entry.substr(len, sizeof(std::uint8_t)), &type1);

    lsm::ValueType type = static_cast<lsm::ValueType>(type1);
    switch (type) {
      case lsm::ValueType::kTypeValue: {
        len += util::DecodeBytes(entry.substr(len), value);
        *status = comm::Status::OK();
      } break;
      case lsm::ValueType::kTypeDeletion: {
        *status = comm::Status(comm::ErrorCode::kNotFound, "Not Found, " + std::string(entry));
      } break;
    }

    return true;
  }

  return false;
}
}  // namespace amkv::table