#include "lsm/sstable_resolver.h"

#include <fcntl.h>

#include "comm/options.h"
#include "gtest/gtest.h"
#include "util/access_file.h"
#include "util/file.h"

namespace amkv::table {
class SSTableResolverTest : public testing::Test {};

TEST_F(SSTableResolverTest, Resolve) {
  util::PosixAccessFile* file = new util::PosixAccessFile("/tmp/sstable_test.txt");

  comm::Status status = file->Open(O_RDONLY | O_CLOEXEC);

  comm::Options options;

  SSTableResolver resolver(options, file);
  resolver.Finish();

  file->Close();
  delete file;
}

}  // namespace amkv::table