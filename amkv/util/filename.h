
#include <cstdint>
#include <string>

namespace amkv::util {
enum FileType {
  kLogFile,
  kDBLockFile,
  kTableFile,
  kDescriptorFile,
  kCurrentFile,
  kTempFile,
  kInfoLogFile  // Either the current one, or an old one
};

std::string LogFileName(const std::string& dbname, const std::uint64_t number);

std::string TableFileName(const std::string& dbname, const std::uint64_t number);

std::string SSTableFileName(const std::string& dbname, const std::uint64_t number);

bool ParseFileName(const std::string& filename, std::uint64_t* number, FileType* type);
}  // namespace amkv::util