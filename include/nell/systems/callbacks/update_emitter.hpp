#pragma once
#include <callbacks/callback_func_handle.hpp>
#include <callbacks/update_callback.hpp>
#include <vector>

namespace nell
{
class UpdateEmitter
{
public:

  unsigned addUpdateCallback(UpdateCallback &callback);
  void removeUpdateCallback(unsigned handle);

protected:
  std::vector<CallbackFuncHandle<UpdateCallback>> _update_callbacks;
};
}  // namespace nell