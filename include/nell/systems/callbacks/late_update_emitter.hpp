#pragma once
#include <callbacks/callback_func_handle.hpp>
#include <callbacks/late_update_callback.hpp>
#include <vector>

namespace nell
{
class LateUpdateEmitter
{
 public:
  unsigned addLateUpdateCallback(LateUpdateCallback &callback);
  void removeLateUpdateCallback(unsigned handle);

 protected:
  std::vector<CallbackFuncHandle<LateUpdateCallback>> _late_update_callbacks;
};
}  // namespace nell