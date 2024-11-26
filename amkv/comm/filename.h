#include <cstdint>
#include <string>

namespace amkv::comm {
std::string LogFileName(const std::string& dbname, std::uint64_t number);
std::string TableFileName(const std::string& dbname, std::uint64_t number);
std::string CurrentFileName(const std::string& dbname);
std::string DescriptorFileName(const std::string& dbname, std::uint64_t number);
std::string TempFileName(const std::string& dbname, std::uint64_t number);
}  // namespace amkv::comm