#pragma once

#include <iostream>

#include "comm/env.h"
#include "lsm/comparator.h"

namespace amkv::comm {
struct Options {
  Options();

  Env* env;
  const lsm::Comparator* comparator;

  std::size_t block_size = 4 * 1024;
  bool create_if_missing = false;
};

struct WriteOptions {
  WriteOptions() = default;
};

struct ReadOptions {
  ReadOptions() = default;
};
}  // namespace amkv::comm