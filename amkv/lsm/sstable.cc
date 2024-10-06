#include "lsm/sstable.h"

#include <fcntl.h>

#include <iostream>

#include "lsm/memtable_iter.h"
#include "lsm/sstable_builder.h"

namespace amkv::table {
std::uint8_t SSTable::Open() {
  std::cout << "DB::Open" << std::endl;
  return 0;
}

comm::Status SSTable::BuildTable(const std::string& fname, const table::MemTable* const memtable) {
  util::WritableFile* file = new util::PosixWritableFile(fname);
  file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);
  comm::Options options;

  table::SSTableBuilder* builder = new table::SSTableBuilder(options, file);
  table::MemTableIterator iter(memtable);
  iter.SeekToFirst();
  for (; iter.Valid(); iter.Next()) {
    std::string_view key = iter.Key();
    std::string_view value = iter.Value();
    builder->Add(key, value);
  }

  comm::Status status = builder->Finish();
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

comm::Status SSTable::FindTable(const std::string& fname) {}
}  // namespace amkv::table