#include <string_view>

#include "comm/iterator.h"
#include "comm/status.h"
#include "memtable.h"
#include "skiplist_iter.h"

namespace amkv::table {
class MemTableIterator : public comm::Iterator<std::string, std::string> {
 public:
  MemTableIterator(const MemTable* memtable);
  ~MemTableIterator() override;

  [[nodiscard]] bool Valid() const override;
  void SeekToFirst() override;
  void SeekToLast() override;
  void Seek(const std::string& target) override;
  void Next() override;
  void Prev() override;
  std::string Key() const override;
  std::string Value() const override;
  comm::Status Status() const override;

 private:
  lsm::SkipListIterator<const std::string_view, const std::string_view>* iter_;
  std::string tmp_;
};
}  // namespace amkv::table