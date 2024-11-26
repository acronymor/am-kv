#include "comm/env.h"

#include <fcntl.h>
#include <util/file.h>

namespace amkv::comm {
Status Env::NewWritableFile(const std::string& filename, util::WritableFile** result) {
  *result = new util::PosixWritableFile(filename);
  (*result)->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);
  return Status::OK();
};

Status Env::NewSequentialFile(const std::string& filename, util::SequentialFile** result) {
  *result = new util::PosixSequentialFile(filename);
  Status status = (*result)->Open(O_RDONLY | O_CLOEXEC);
  return status;
};

Status Env::NewRandomAccessFile(const std::string& filename, util::AccessFile** result) {
  *result = new util::PosixAccessFile(filename);
  Status status = (*result)->Open(O_RDONLY | O_CLOEXEC);
  return status;
}

Status Env::WriteStringToFileAsync(const std::string& filename, std::string_view data) { return Status::OK(); }

Status Env::WriteStringToFileSync(const std::string& filename, std::string_view data) {
  util::WritableFile* file;
  Status status = this->NewWritableFile(filename, &file);
  if (ErrorCode::kOk != status.Code()) {
    return status;
  }
  status = file->Append(data);
  if (ErrorCode::kOk == status.Code()) {
    status = file->Sync();
  }
  if (ErrorCode::kOk == status.Code()) {
    status = file->Close();
  }
  delete file;  // Will auto-close if we did not close above
  if (ErrorCode::kOk != status.Code()) {
    util::DeleteFile(filename);
  }
  return status;
}

Status Env::ReadStringFromFileSync(const std::string& filename, std::string* data) {
  data->clear();
  util::SequentialFile* file{nullptr};
  Status status = this->NewSequentialFile(filename, &file);
  if (ErrorCode::kOk != status.Code()) {
    return status;
  }

  constexpr int kBufferSize = 8192;
  char* space = new char[kBufferSize];
  while (true) {
    std::string_view fragment;
    status = file->Read(kBufferSize, &fragment, space);
    if (ErrorCode::kOk != status.Code()) {
      break;
    }

    data->append(fragment);
    if (fragment.empty()) {
      break;
    }
  }

  delete[] space;
  delete file;

  return status;
}
}  // namespace amkv::comm