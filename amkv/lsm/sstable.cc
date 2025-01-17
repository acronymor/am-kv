#include "lsm/sstable.h"

#include <fcntl.h>

#include "block_iter.h"
#include "lsm/memtable_iter.h"
#include "lsm/sstable_builder.h"
#include "sstable_resolver.h"
#include "util/access_file.h"

namespace amkv::table {
comm::Status SSTable::Put(const std::string& fname, const MemTable* const memtable, version::FileMetaData* meta) {
  util::WritableFile* file = new util::PosixWritableFile(fname);
  file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);
  comm::Options options;

  SSTableBuilder* builder = new SSTableBuilder(options, file);
  MemTableIterator iter(memtable);
  iter.SeekToFirst();

  meta->smallest = iter.Key();

  for (; iter.Valid(); iter.Next()) {
    const std::string key = iter.Key();
    const std::string value = iter.Value();
    builder->Add(key, value);
    meta->largest = iter.Key();
  }

  comm::Status status = builder->Finish();
  meta->file_size = builder->FileSize();

  delete builder;

  if (status.Code() == comm::ErrorCode::kOk) {
    status = file->Sync();
  }
  if (status.Code() == comm::ErrorCode::kOk) {
    status = file->Close();
  }

  delete file, file = nullptr;
  return status;
}

comm::Status SSTable::Get(const std::string& fname, const std::string_view key, std::string* out) {
  util::AccessFile* file = new util::PosixAccessFile(fname);
  comm::Status status = file->Open(O_RDONLY | O_CLOEXEC);
  std::string footer_input;
  comm::Options options;

  SSTableResolver* resolver = new SSTableResolver(options, file);
  status = resolver->Finish();
  const auto rep = resolver->Rep();

  // TODO I can quickly locate DataBlock based on the IndexBlock
  std::string data_content;
  status = file->Read(rep->data_block_handler.GetOffset(), rep->data_block_handler.GetSize(), &data_content);
  block::Block block(data_content);

  block::BlockIterator iter(&block);
  iter.SeekToFirst();

  std::string kkey = std::string(key);
  iter.Seek(kkey);
  *out = iter.Value();

  return status;
}
}  // namespace amkv::table