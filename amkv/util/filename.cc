#include "util/filename.h"

#include <format>

namespace amkv::util {
static std::string MANIFEST = "MANIFEST-";

static std::string MakeFileName(const std::string& dbname, const std::uint64_t number, const std::string& suffix) {
  std::string filename = std::format("{0}/{1:06}.{2}", dbname, number, suffix);
  return filename;
}

std::string LogFileName(const std::string& dbname, const std::uint64_t number) {
  return MakeFileName(dbname, number, "log");
}

std::string TableFileName(const std::string& dbname, const std::uint64_t number) {
  return MakeFileName(dbname, number, "ldb");
}

std::string SSTableFileName(const std::string& dbname, const std::uint64_t number) {
  return MakeFileName(dbname, number, "sst");
}

bool ParseFileName(const std::string& filename, std::uint64_t* number, FileType* type) {
  if (filename == "CURRENT") {
    *number = 0;
    *type = kCurrentFile;
  } else if (filename == "LOCK") {
    *number = 0;
    *type = kDBLockFile;
  } else if (filename == "LOG" || filename == "LOG.old") {
    *number = 0;
    *type = kInfoLogFile;
  } else if (filename.starts_with(MANIFEST)) {
    std::string num = filename.substr(MANIFEST.size(), filename.size());
    *type = kDescriptorFile;
    *number = stol(num);
  } else {
    std::size_t pos = filename.rfind('.');
    std::string suffix = filename.substr(pos, filename.size());
    if (suffix == ".log") {
      *number = stol(filename.substr(0, pos));
      *type = kLogFile;
    } else if (suffix == ".sst") {
      *number = stol(filename.substr(0, pos));
      *type = kTableFile;
    } else if (suffix == ".ldb") {
      *number = stol(filename.substr(0, pos));
      *type = kTableFile;
    } else if (suffix == ".dbtmp") {
      *number = stol(filename.substr(0, pos));
      *type = kTempFile;
    } else {
      return false;
    }
  }
  return true;
}

}  // namespace amkv::util