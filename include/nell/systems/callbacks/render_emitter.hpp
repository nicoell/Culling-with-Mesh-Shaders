#pragma once
#include <callbacks/callback_func_handle.hpp>
#include <callbacks/render_callback.hpp>
#include <vector>

namespace nell
{
class RenderEmitter
{
 public:
  unsigned addRenderCallback(RenderCallback &callback);
  void removeRenderCallback(unsigned handle);

 protected:
  std::vector<CallbackFuncHandle<RenderCallback>> _render_callbacks;
};
}  // namespace nell