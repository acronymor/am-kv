#include "comm/options.h"
#include "lsm/block_builder.h"
#include "lsm/block_handler.h"
#include "lsm/sstable_rep.h"
#include "util/access_file.h"
#include "util/writable_file.h"

namespace amkv::table {
class SSTableBuilder {
 public:
  SSTableBuilder(const comm::Options& options, util::WritableFile* file);
  ~SSTableBuilder();

  SSTableBuilder(const SSTableBuilder&) = delete;
  SSTableBuilder& operator=(const SSTableBuilder&) = delete;

  comm::Status Finish();

  void Add(const std::string_view key, const std::string_view value);

  comm::Status Status() const;

  std::size_t FileSize() const;

 private:
  void flush();

  void writeBlock(block::BlockBuilder* block, block::BlockHandler* handler);
  void writeRawBlock(const std::string_view block_content, block::BlockHandler* handler);

  SSTableBuilderRep* rep_;
};
}  // namespace amkv::table
