#include "db/write_batch_internal.h"

#include <util/codec.h>

namespace amkv::db {
void WriteBatchInternal::SetCount(WriteBatch& batch, std::uint32_t count) {
  util::EncodeUInt32(count, &batch.rep_);
}

std::uint32_t WriteBatchInternal::Count(const WriteBatch* batch) { return 0; }

void WriteBatchInternal::SetSequence(WriteBatch* batch, SequenceNumber seq) { util::EncodeUInt64(seq, &batch->rep_); }

SequenceNumber WriteBatchInternal::Sequence(WriteBatch* batch) {
  std::uint64_t seq = 0;
  util::DecodeUInt64(batch->rep_, &seq);
  return seq;
}

std::string_view WriteBatchInternal::Contents(const WriteBatch* batch) { return std::string_view(batch->rep_); }

util::Status WriteBatchInternal::InsertInto(const WriteBatch* batch) {
  return util::Status::OK();
}

}  // namespace amkv::db