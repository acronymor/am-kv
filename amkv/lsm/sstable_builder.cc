#include "lsm/sstable_builder.h"

#include "util/codec.h"

namespace amkv::table {

SSTableBuilder::SSTableBuilder(const comm::Options& options, util::WritableFile* file)
    : rep_(new SSTableBuilderRep(&options, file)){};

SSTableBuilder::~SSTableBuilder() { delete this->rep_; }

comm::Status SSTableBuilder::Finish() {
  this->flush();
  SSTableBuilderRep* rep = this->rep_;

  block::BlockHandler filter_block_handler;
  this->writeBlock(&rep->filter_block, &filter_block_handler);

  std::string key = "filter";
  std::string filter_handle_encoding;
  filter_block_handler.EncodeTo(&filter_handle_encoding);
  rep->meta_index_block.Add(key, filter_handle_encoding);
  block::BlockHandler metaindex_block_handler;
  this->writeBlock(&rep->meta_index_block, &metaindex_block_handler);

  if (rep->pending_index_entry) {
    std::string index_handle_encoding;
    rep->pending_handle.EncodeTo(&index_handle_encoding);
    rep->index_block.Add(rep->last_key, index_handle_encoding);
    rep->pending_index_entry = false;
  }
  block::BlockHandler index_block_handler;
  this->writeBlock(&rep->index_block, &index_block_handler);

  std::string foot_handle_encoding;
  metaindex_block_handler.EncodeTo(&foot_handle_encoding);
  index_block_handler.EncodeTo(&foot_handle_encoding);
  rep->footer_block.Add(foot_handle_encoding, "");

  block::BlockHandler foot_block_handler;
  this->writeBlock(&rep->footer_block, &foot_block_handler);
  rep->status = comm::Status::OK();

  return rep->status;
}

void SSTableBuilder::Add(const std::string_view key, const std::string_view value) {
  SSTableBuilderRep* rep = this->rep_;

  std::string tmp_key;
  std::string tmp_value;
  util::EncodeBytes(key, &tmp_key);
  util::EncodeBytes(value, &tmp_value);
  rep->data_block.Add(tmp_key, tmp_value);

  if (rep->pending_index_entry) {
    std::string index_handle_encoding;
    rep->pending_handle.EncodeTo(&index_handle_encoding);
    rep->index_block.Add(rep->last_key, index_handle_encoding);
  }

  rep->filter_block.Add(key, "");

  rep->last_key.assign(key.data(), key.size());

  if (rep->data_block.Size() >= 4 * 1024) {
    this->flush();
  }
}

void SSTableBuilder::flush() {
  SSTableBuilderRep* rep = this->rep_;

  if (this->Status().Code() != comm::ErrorCode::kOk) {
    return;
  }

  if (rep->data_block.Empty()) {
    return;
  }

  this->writeBlock(&rep->data_block, &rep->pending_handle);

  // TODO
  if (this->Status().Code() == comm::ErrorCode::kOk) {
    rep->pending_index_entry = true;
    rep->status = rep->file->Flush();
  }
}

comm::Status SSTableBuilder::Status() const { return this->rep_->status; }
std::size_t SSTableBuilder::FileSize() const { return this->rep_->offset; }

void SSTableBuilder::writeBlock(block::BlockBuilder* block, block::BlockHandler* handler) {
  std::string_view raw = block->Build();
  std::string_view block_content = raw;
  this->writeRawBlock(block_content, handler);
  block->Reset();
}

void SSTableBuilder::writeRawBlock(const std::string_view block_content, block::BlockHandler* handler) {
  const std::string crc32 = "crc32";
  SSTableBuilderRep* rep = this->rep_;
  handler->SetOffset(rep->offset);
  handler->SetSize(block_content.size());
  rep->status = rep->file->Append(block_content);
  if (rep->status.Code() == comm::ErrorCode::kOk) {
    rep->status = rep->file->Append(crc32);
    if (rep->status.Code() == comm::ErrorCode::kOk) {
      rep->offset += block_content.size() + crc32.length();
    }
  }
}
}  // namespace amkv::table