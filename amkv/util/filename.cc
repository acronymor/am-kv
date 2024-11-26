#include "util/filename.h"
#include <format>

namespace amkv::util {
static std::string MakeFileName(const std::string& dbname, const std::uint64_t number, const std::string& suffix) {
  std::string filename = std::format("{0}/{1:06}.{2}", dbname, number, suffix);
  return filename;
}

std::string LogFileName(const std::string& dbname, const std::uint64_t number) { return MakeFileName(dbname, number, "log"); }

std::string TableFileName(const std::string& dbname, const std::uint64_t number) {
  return MakeFileName(dbname, number, "ldb");
}

std::string SSTableFileName(const std::string& dbname, const std::uint64_t number) {
  return MakeFileName(dbname, number, "sst");
}

}  // namespace amkv::util