#include "util/dirname.h"

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
}