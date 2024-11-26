#include "comm/status.h"

namespace amkv::util {
comm::Status GetFileSize(const std::string& filename, std::size_t* size);
comm::Status DeleteFile(const std::string& filename);
bool FileExists(const std::string& filename);
comm::Status RemoveDir(const std::string& filename);
comm::Status CreateDir(const std::string& dirname);
comm::Status RenameFile(const std::string& from, const std::string& to);
}  // namespace amkv::util