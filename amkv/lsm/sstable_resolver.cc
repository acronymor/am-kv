#include "lsm/sstable_resolver.h"

#include "util/codec.h"

namespace amkv::table {
SSTableResolver::SSTableResolver(const comm::Options& options, util::AccessFile* file)
    : rep_(new SSTableResolverRep(&options, file)) {}

SSTableResolver::~SSTableResolver() { delete rep_; }

const SSTableResolverRep* const SSTableResolver::Rep() const { return rep_; }

comm::Status SSTableResolver::Finish() {
  SSTableResolverRep* rep = this->rep_;
  std::size_t len = 0;

  std::size_t file_size = rep_->file->Size();
  std::string crc32 = "crc32";
  len = crc32.size();

  std::size_t footer_size = sizeof(uint64_t) * 2 + sizeof(uint64_t) * 2;
  std::size_t footer_offset = file_size - len - footer_size;
  rep->footer_block_handler.SetOffset(footer_offset);
  rep->footer_block_handler.SetSize(footer_size);

  std::string out = "";
  len = 0;
  std::uint64_t metaindex_block_offset = 0;
  rep->file->Read(footer_offset + len, sizeof(metaindex_block_offset), &out);
  len += util::DecodeUInt64(out, &metaindex_block_offset);
  rep->meta_index_block_handler.SetOffset(metaindex_block_offset);

  out = "";
  std::uint64_t metaindex_block_size = 0;
  rep->file->Read(footer_offset + len, sizeof(metaindex_block_size), &out);
  len += util::DecodeUInt64(out, &metaindex_block_size);
  rep->meta_index_block_handler.SetSize(metaindex_block_size);

  out = "";
  std::uint64_t index_block_offset = 0;
  rep->file->Read(footer_offset + len, sizeof(index_block_offset), &out);
  len += util::DecodeUInt64(out, &index_block_offset);
  rep->index_block_handler.SetOffset(index_block_offset);

  out = "";
  std::uint64_t index_block_size = 0;
  rep->file->Read(footer_offset + len, sizeof(index_block_size), &out);
  len += util::DecodeUInt64(out, &index_block_size);
  rep->index_block_handler.SetSize(index_block_size);

  out = "";
  std::string key = "filter";
  len = key.size();
  std::uint64_t filter_block_offset = 0;
  rep->file->Read(metaindex_block_offset + len, sizeof(filter_block_offset), &out);
  len += util::DecodeUInt64(out, &filter_block_offset);
  rep->filter_block_handler.SetOffset(filter_block_offset);

  out = "";
  std::uint64_t filter_block_size = 0;
  rep->file->Read(metaindex_block_offset + len, sizeof(filter_block_size), &out);
  len += util::DecodeUInt64(out, &filter_block_size);
  rep->filter_block_handler.SetSize(filter_block_size);

  rep->data_block_handler.SetOffset(0);
  rep->data_block_handler.SetSize(filter_block_offset - crc32.size());

  return comm::Status::OK();
}

}  // namespace amkv::table
