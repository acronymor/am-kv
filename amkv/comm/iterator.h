#pragma once

#include "comm/status.h"

namespace amkv::comm {
template <typename K, typename V>
class Iterator {
 public:
  Iterator() = default;
  virtual ~Iterator() = default;

  Iterator(const Iterator&) = delete;
  Iterator& operator=(const Iterator&) = delete;

  [[nodiscard]] virtual bool Valid() const = 0;
  virtual void SeekToFirst() = 0;
  virtual void SeekToLast() = 0;
  virtual void Seek(const K& target) = 0;
  virtual void Next() = 0;
  virtual void Prev() = 0;
  virtual K Key() const = 0;
  virtual V Value() const = 0;
  virtual comm::Status Status() const = 0;
};
}  // namespace amkv::comm