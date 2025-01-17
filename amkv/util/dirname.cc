#include "util/dirname.h"

#include <dirent.h>

namespace amkv::util {
std::string Basename(const std::string& filename) {
  std::string::size_type separator_pos = filename.rfind('/');
  if (separator_pos == std::string::npos) {
    return filename;
  }
  return filename.substr(separator_pos + 1, filename.length() - separator_pos - 1);
}

std::string Dirname(const std::string& filename) {
  std::string::size_type separator_pos = filename.rfind('/');
  if (separator_pos == std::string::npos) {
    return ".";
  }
  return filename.substr(0, separator_pos);
}

std::vector<std::string> ListDir(const std::string& dirname) {
  std::vector<std::string> result;
  ::DIR* dir = ::opendir(dirname.c_str());
  if (dir == nullptr) {
    return result;
  }
  struct ::dirent* entry;
  while ((entry = ::readdir(dir)) != nullptr) {
    if (entry->d_type == DT_CHR) {
      continue;
    }
    result.emplace_back(entry->d_name);
  }
  ::closedir(dir);
  return result;
};
}  // namespace amkv::util