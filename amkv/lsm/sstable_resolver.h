#include "comm/options.h"
#include "lsm/sstable_rep.h"
#include "util/access_file.h"

namespace amkv::table {
class SSTableResolver {
 public:
  SSTableResolver(const comm::Options& options, util::AccessFile* file);
  ~SSTableResolver();

  SSTableResolver(const SSTableResolver&) = delete;
  SSTableResolver& operator=(const SSTableResolver&) = delete;

  [[nodiscard]] const SSTableResolverRep* const Rep() const;
  comm::Status Finish();

 private:
  SSTableResolverRep* rep_;
};
};  // namespace amkv::table
