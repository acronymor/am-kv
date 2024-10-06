#include "util/filename.h"

namespace amkv::util {
static std::string MakeFileName(const std::string& dbname, const std::uint64_t number, const std::string& suffix) {
  return dbname + "-" + std::to_string(number) + "." + suffix;
}

std::string LogFileName(const std::string& dbname, const std::uint64_t number) { return MakeFileName(dbname, number, "log"); }

std::string TableFileName(const std::string& dbname, const std::uint64_t number) {
  return MakeFileName(dbname, number, "ldb");
}

std::string SSTableFileName(const std::string& dbname, const std::uint64_t number) {
  return MakeFileName(dbname, number, "sst");
}

}  // namespace amkv::util