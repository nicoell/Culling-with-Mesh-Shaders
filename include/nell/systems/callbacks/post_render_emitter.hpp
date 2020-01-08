#pragma once
#include <callbacks/callback_func_handle.hpp>
#include <callbacks/post_render_callback.hpp>
#include <vector>

namespace nell
{
class PostRenderEmitter
{
 public:
  unsigned addPostRenderCallback(PostRenderCallback &callback);
  void removePostRenderCallback(unsigned handle);

 protected:
  std::vector<CallbackFuncHandle<PostRenderCallback>> _post_render_callbacks;
};
}  // namespace nell