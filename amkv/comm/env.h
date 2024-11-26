#pragma once

#include "comm/status.h"
#include "util/access_file.h"
#include "util/sequential_file.h"
#include "util/writable_file.h"

namespace amkv::comm {
class Env {
 public:
  Env() = default;
  Env(const Env&) = delete;
  Env& operator=(const Env&) = delete;

  ~Env() = default;

  Status NewWritableFile(const std::string& filename, util::WritableFile** result);

  Status NewSequentialFile(const std::string& filename, util::SequentialFile** result);

  Status NewRandomAccessFile(const std::string& filename, util::AccessFile** result);

  Status WriteStringToFileAsync(const std::string& filename, std::string_view data);

  Status WriteStringToFileSync(const std::string& filename, std::string_view data);

  Status ReadStringFromFileSync(const std::string& filename, std::string* data);
};
}  // namespace amkv::comm
