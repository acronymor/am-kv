#include "util/file.h"

#include <sys/stat.h>
#include <unistd.h>

namespace amkv::util {
comm::Status GetFileSize(const std::string& filename, std::size_t* size) {
  struct ::stat file_stat;
  if (::stat(filename.c_str(), &file_stat) != 0) {
    *size = 0;
    return comm::Status(comm::ErrorCode::kFileError, "File Size got failed");
  }
  *size = file_stat.st_size;
  return comm::Status::OK();
}

comm::Status DeleteFile(const std::string& filename) {
  if (::unlink(filename.c_str()) != 0) {
    return comm::Status(comm::ErrorCode::kFileError, "Delete File failed");
  }

  return comm::Status::OK();
}

bool FileExists(const std::string& filename) { return ::access(filename.c_str(), F_OK) == 0; }

comm::Status RemoveDir(const std::string& filename) {
  if (::rmdir(filename.c_str()) != 0) {
    return comm::Status(comm::ErrorCode::kDirError, "Remove Dir failed");
  }

  return comm::Status::OK();
}

comm::Status CreateDir(const std::string& dirname) {
  if (::mkdir(dirname.c_str(), 0755) != 0) {
    return comm::Status(comm::ErrorCode::kDirError, "Create Dir failed");
  }
  return comm::Status::OK();
}

comm::Status RenameFile(const std::string& from, const std::string& to) {
  if (std::rename(from.c_str(), to.c_str()) != 0) {
    return comm::Status(comm::ErrorCode::kFileError, "Rename File failed");
  }
  return comm::Status::OK();
}
}  // namespace amkv::util
