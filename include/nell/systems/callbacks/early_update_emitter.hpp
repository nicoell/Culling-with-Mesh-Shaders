#pragma once
#include <callbacks/callback_func_handle.hpp>
#include <callbacks/early_update_callback.hpp>
#include <vector>

namespace nell
{
class EarlyUpdateEmitter
{
 public:
  unsigned addEarlyUpdateCallback(EarlyUpdateCallback &callback);
  void removeEarlyUpdateCallback(unsigned handle);

 protected:
  std::vector<CallbackFuncHandle<EarlyUpdateCallback>> _early_update_callbacks;
};
}  // namespace nell