#include "comm/iterator.h"
#include "lsm/block.h"

namespace amkv::block {
class BlockIterator : public comm::Iterator<std::string, std::string> {
 public:
  BlockIterator(const Block* block);
  ~BlockIterator() override;

  bool Valid() const override;
  void SeekToFirst() override;
  void SeekToLast() override;
  void Seek(const std::string& target) override;
  void Next() override;
  void Prev() override;
  std::string Key() const override;
  std::string Value() const override;
  comm::Status Status() const override;

 private:
  std::uint64_t nextEntryOffSet() const;

  const Block* block_;
  std::size_t offset_;
};
}  // namespace amkv::block