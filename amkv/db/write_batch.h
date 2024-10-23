#pragma once

#include <string>
#include <string_view>

#include "comm/status.h"
#include "db/write_batch_handler.h"

namespace amkv::db {

class WriteBatch {
 friend class WriteBatchInternal;

 public:
  void Put(const std::string_view key, const std::string_view value);

  void Delete(const std::string_view key);

  comm::Status Iterate(WriteBatchHandler* handler);

 private:
  std::string rep_;
};
}  // namespace amkv::db