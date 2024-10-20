#include "util/file.h"

#include <sys/stat.h>

namespace amkv::util {
comm::Status GetFileSize(const std::string& filename, std::size_t* size) {
  struct ::stat file_stat;
  if (::stat(filename.c_str(), &file_stat) != 0) {
    *size = 0;
    return comm::Status(amkv::comm::ErrorCode::kFileError, "File Size got failed");
  }
  *size = file_stat.st_size;
  return comm::Status::OK();
}

}  // namespace amkv::util
