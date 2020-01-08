#include <systems/callbacks/post_render_emitter.hpp>

unsigned nell::PostRenderEmitter::addPostRenderCallback (
  PostRenderCallback &callback)
{
  unsigned handle = (_post_render_callbacks.empty())
                        ? 0
                        : _post_render_callbacks.back().handle + 1;

  _post_render_callbacks.emplace_back(handle, callback);
  return handle;
}

void nell::PostRenderEmitter::removePostRenderCallback (unsigned handle) {
  auto pos = std::find(_post_render_callbacks.begin(),
                       _post_render_callbacks.end(), handle);
  if (pos != _post_render_callbacks.end()) _post_render_callbacks.erase(pos);

}
