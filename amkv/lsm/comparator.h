#pragma once

#include <cstdint>
#include <string_view>

namespace amkv::lsm {
struct Comparator {
  virtual ~Comparator() {};

  virtual std::int64_t Compare(const std::string_view lhs, const std::string_view rhs) const = 0;

  virtual const std::string Name() const = 0;
};

class InternalKeyComparator : public Comparator {
 public:
  InternalKeyComparator(const Comparator* comparator);
  ~InternalKeyComparator();

  std::int64_t Compare(const std::string_view lhs, const std::string_view rhs) const override;

  const std::string Name() const override;

 private:
  const Comparator* comparator_;
};

class BytewiseComparator : public Comparator {
 public:
  BytewiseComparator() = default;
  ~BytewiseComparator();

  std::int64_t Compare(const std::string_view lhs, const std::string_view rhs) const override;

  const std::string Name() const override;
};

struct MemTableKeyComparator {
  const InternalKeyComparator comparator;

  explicit MemTableKeyComparator(const InternalKeyComparator& comparator);

  std::int64_t operator()(const std::string_view lhs, const std::string_view rhs) const;
};
}  // namespace amkv::lsm
