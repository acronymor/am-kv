#include "comm/status.h"

namespace amkv::util {
comm::Status GetFileSize(const std::string& filename, std::size_t* size);

}  // namespace amkv::util