#include <systems/callbacks/update_emitter.hpp>

unsigned nell::UpdateEmitter::addUpdateCallback (
  UpdateCallback &callback)
{
  unsigned handle = (_update_callbacks.empty())
                        ? 0
                        : _update_callbacks.back().handle + 1;

  _update_callbacks.emplace_back(handle, callback);
  return handle;
}

void nell::UpdateEmitter::removeUpdateCallback (unsigned handle) {
  auto pos = std::find(_update_callbacks.begin(),
                       _update_callbacks.end(), handle);
  if (pos != _update_callbacks.end()) _update_callbacks.erase(pos);

}
