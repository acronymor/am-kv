#include "db/write_batch.h"

namespace amkv::db {
struct Writer {
  Writer();

  WriteBatch* batch;

  bool done;
};
}  // namespace amkv::db