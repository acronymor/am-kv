#include "comm/env.h"
#include "comm/status.h"
#include "util/file.h"

namespace amkv::version {
inline comm::Status SetCurrentFile(comm::Env* env, const std::string& dbname, std::uint64_t number) {
  std::string tmp_file = comm::TempFileName(dbname, number);
  std::string manifest = comm::DescriptorFileName(dbname, number);
  manifest = manifest.substr(manifest.find("/") + 1);
  comm::Status status = env->WriteStringToFileSync(tmp_file, manifest);
  if (status.Code() == comm::ErrorCode::kOk) {
    status = util::RenameFile(tmp_file, comm::CurrentFileName(dbname));
  }
  if (status.Code() != comm::ErrorCode::kOk) {
    util::DeleteFile(tmp_file);
  }
  return status;
};
};  // namespace amkv::version