#include "comm/filename.h"

#include <format>

namespace amkv::comm {
static std::string MakeFileName(const std::string& dbname, std::uint64_t number, const std::string& suffix) {
  std::string res = std::format("{0}/{1:06d}.{2}", dbname, number, suffix);
  return res;
}

std::string LogFileName(const std::string& dbname, std::uint64_t number) { return MakeFileName(dbname, number, "log"); }

std::string TableFileName(const std::string& dbname, std::uint64_t number) {
  return MakeFileName(dbname, number, "ldb");
}

std::string CurrentFileName(const std::string& dbname) { return dbname + "/CURRENT"; }

std::string DescriptorFileName(const std::string& dbname, std::uint64_t number) {
  std::string res = std::format("{0}/{1}-{2:06d}", dbname, "MANIFEST", number);
  return res;
}

std::string TempFileName(const std::string& dbname, std::uint64_t number) {
  std::string res = MakeFileName(dbname, number, "dbtmp");
  return res;
}
}  // namespace amkv::comm