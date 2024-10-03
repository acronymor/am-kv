#include "lsm/comparator.h"

#include <iostream>

namespace amkv::lsm {

InternalKeyComparator::InternalKeyComparator(const Comparator* comparator) : comparator_(comparator) {}

std::int64_t InternalKeyComparator::Compare(const std::string_view lhs, const std::string_view rhs) const {
  std::cout << "InternalKeyComparator::Compare" << std::endl;
  return lhs.compare(rhs);
}

const std::string InternalKeyComparator::Name() const { return "amkv.InternalKeyComparator"; }

std::int64_t BytewiseComparator::Compare(const std::string_view lhs, const std::string_view rhs) const {
  return lhs.compare(rhs);
}

const std::string BytewiseComparator::Name() const { return "BytewiseComparator::Compare"; }

MemTableKeyComparator::MemTableKeyComparator(const InternalKeyComparator& comparator) : comparator(comparator){};

std::int64_t MemTableKeyComparator::operator()(const std::string_view lhs, const std::string_view rhs) const {
  return lhs.compare(rhs);
};
}  // namespace amkv::lsm