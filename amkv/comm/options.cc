#include "comm/options.h"

namespace amkv::comm {
Options::Options() : comparator(new lsm::BytewiseComparator()), env(new Env()) {}
}  // namespace amkv::comm