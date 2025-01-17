#include <string_view>

namespace amkv::block {
class FilterPolicy {
 public:
  ~FilterPolicy() = default;
  virtual std::string_view Name() = 0;
};

class InternalFilterPolicy : public FilterPolicy {
 public:
  explicit InternalFilterPolicy(const FilterPolicy* policy);
  std::string_view Name() override;

 private:
  const FilterPolicy* const uer_policy_;
};
}  // namespace amkv::block
