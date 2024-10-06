#include "lsm/skiplist.h"

#include "gtest/gtest.h"
#include "lsm/skiplist_iter.h"

namespace amkv::lsm {
class KeyComparator : public Comparator {
 public:
  std::int64_t Compare(const std::string_view lhs, const std::string_view rhs) const override {
    return lhs.compare(rhs);
  }
  const std::string Name() const override { return "KeyComparator"; }

  ~KeyComparator() override = default;
};

TEST(SkipListTest, String) {
  util::Arena arena;
  const Comparator* cmp = new KeyComparator();
  SkipList<std::string_view, std::string_view> list(&arena, cmp);

  std::string_view s1("key1");
  std::string_view s2("key2");
  std::string_view s3("key3");
  list.Insert(s1);
  list.Insert(s2);
  list.Insert(s3);

  SkipListIterator iter(&list);
  iter.SeekToFirst();
  EXPECT_TRUE(iter.Valid());
  EXPECT_EQ(s1, iter.Key());
  EXPECT_TRUE(iter.Valid());
  iter.Next();
  EXPECT_TRUE(iter.Valid());
  EXPECT_EQ(s2, iter.Key());
  EXPECT_TRUE(iter.Valid());
  iter.Next();
  EXPECT_EQ(s3, iter.Key());
}
}  // namespace amkv::lsm