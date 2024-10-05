#include "db/write_batch_internal.h"

namespace amkv::db {
void WriteBatchInternal::SetCount(std::uint32_t count) { this->count_ = count; }

std::uint32_t WriteBatchInternal::Count() { return this->count_; }

void WriteBatchInternal::SetSequence(lsm::SequenceNumber seq) { this->seq_ = seq; }

lsm::SequenceNumber WriteBatchInternal::Sequence() { return this->seq_; }

comm::Status WriteBatchInternal::InsertInto(WriteBatch* batch, table::MemTable* memtable) {
  WriteBatchHandler handler(memtable);
  return batch->Iterate(&handler);
}

}  // namespace amkv::db