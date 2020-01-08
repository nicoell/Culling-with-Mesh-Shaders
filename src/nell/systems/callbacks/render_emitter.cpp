#include <systems/callbacks/render_emitter.hpp>

unsigned nell::RenderEmitter::addRenderCallback (
  RenderCallback &callback)
{
  unsigned handle = (_render_callbacks.empty())
                        ? 0
                        : _render_callbacks.back().handle + 1;

  _render_callbacks.emplace_back(handle, callback);
  return handle;
}

void nell::RenderEmitter::removeRenderCallback (unsigned handle) {
  auto pos = std::find(_render_callbacks.begin(),
                       _render_callbacks.end(), handle);
  if (pos != _render_callbacks.end()) _render_callbacks.erase(pos);

}
