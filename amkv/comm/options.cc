#include "comm/options.h"

namespace amkv::comm {
Options::Options() : comparator(new lsm::BytewiseComparator()) {}
}  // namespace amkv::comm