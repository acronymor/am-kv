#include "db/writer.h"

namespace amkv::db {
Writer::Writer() : batch(nullptr), done(false) {}
}  // namespace amkv::db