#include <string>
#include <vector>

namespace amkv::util {
std::string Basename(const std::string& filename);

std::string Dirname(const std::string& filename);

std::vector<std::string> ListDir(const std::string& dirname);
}  // namespace amkv::util