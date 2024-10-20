#pragma once

#include "comm/options.h"
#include "lsm/block_builder.h"
#include "lsm/block_handler.h"
#include "util/access_file.h"
#include "util/writable_file.h"

namespace amkv::table {

struct SSTableBuilderRep {
  SSTableBuilderRep(const comm::Options* options, util::WritableFile* file)
      : options(options), file(file), status(comm::ErrorCode::kOk, ""){};

  const comm::Options* options;
  util::WritableFile* file;

  std::string last_key;

  std::uint64_t offset{0};
  comm::Status status;

  block::BlockHandler pending_handle;
  bool pending_index_entry{false};

  block::BlockBuilder data_block;
  block::BlockBuilder filter_block;
  block::BlockBuilder meta_index_block;
  block::BlockBuilder index_block;
  block::BlockBuilder footer_block;
};

struct SSTableResolverRep {
  SSTableResolverRep(const comm::Options* options, util::AccessFile* file)
      : options(options), file(file), status(comm::ErrorCode::kOk, ""){};

  comm::Status status;

  const comm::Options* options;
  util::AccessFile* file;

  block::BlockHandler data_block_handler;
  block::BlockHandler filter_block_handler;
  block::BlockHandler meta_index_block_handler;
  block::BlockHandler index_block_handler;
  block::BlockHandler footer_block_handler;
};
}  // namespace amkv::table
