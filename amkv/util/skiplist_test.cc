#include "util/skiplist.h"

#include "gtest/gtest.h"

namespace amkv::util {

template <typename Key>
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

TEST(SkipListTest, String) {
  Arena arena;
  Comparator<std::string_view> cmp;
  SkipList<std::string_view, Comparator<std::string_view>> list(&arena, cmp);

  std::string_view s1("key1");
  std::string_view s2("key2");
  std::string_view s3("key3");
  list.Insert(s1);
  list.Insert(s2);
  list.Insert(s3);

  SkipList<std::string_view, Comparator<std::string_view>>::Iterator iter(&list);
  iter.SeekToFirst();
  EXPECT_TRUE(iter.Valid());
  EXPECT_EQ(s1, iter.key());
  EXPECT_TRUE(iter.Valid());
  iter.Next();
  EXPECT_TRUE(iter.Valid());
  EXPECT_EQ(s2, iter.key());
  EXPECT_TRUE(iter.Valid());
  iter.Next();
  EXPECT_EQ(s3, iter.key());
}
}  // namespace amkv::util