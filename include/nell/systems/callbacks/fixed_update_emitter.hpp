#pragma once
#include <callbacks/callback_func_handle.hpp>
#include <callbacks/fixed_update_callback.hpp>
#include <vector>

namespace nell
{
class FixedUpdateEmitter
{
 public:
  unsigned addFixedUpdateCallback(FixedUpdateCallback &callback);
  void removeFixedUpdateCallback(unsigned handle);

 protected:
  std::vector<CallbackFuncHandle<FixedUpdateCallback>> _fixed_update_callbacks;
};
}  // namespace nell