#include "util/skiplist.h"

#include "gtest/gtest.h"

namespace amkv::util {

typedef uint64_t Key;
struct Comparator {
  int operator()(const Key& a, const Key& b) const {
    if (a < b) {
      return -1;
    } else if (a > b) {
      return +1;
    } else {
      return 0;
    }
  }
};

TEST(SkipListTest, Simple) {
  util::Arena arena;
  Comparator cmp;
  SkipList<Key, Comparator> list(&arena, cmp);

  const std::size_t cnt = 10;

  for (std::size_t i = 0; i < cnt; i++) {
    list.Insert(i * 3);
  }

  SkipList<Key, Comparator>::Iterator iter(&list);
  iter.SeekToFirst();
  std::size_t idx = 0;
  while (iter.Valid()) {
    ASSERT_EQ(iter.key(), 3 * idx++);
    iter.Next();
  }
}
}  // namespace amkv::table