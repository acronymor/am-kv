#include "lsm/policy.h"

namespace amkv::block {
InternalFilterPolicy::InternalFilterPolicy(const FilterPolicy* policy) : uer_policy_(policy) {}

std::string_view InternalFilterPolicy::Name() { return {"InternalFilterPolicy"}; }

}  // namespace amkv::block