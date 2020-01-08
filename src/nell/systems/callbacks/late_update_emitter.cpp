#include <systems/callbacks/late_update_emitter.hpp>

unsigned nell::LateUpdateEmitter::addLateUpdateCallback (
  LateUpdateCallback &callback)
{
  unsigned handle = (_late_update_callbacks.empty())
                        ? 0
                        : _late_update_callbacks.back().handle + 1;

  _late_update_callbacks.emplace_back(handle, callback);
  return handle;
}

void nell::LateUpdateEmitter::removeLateUpdateCallback (unsigned handle) {
  auto pos = std::find(_late_update_callbacks.begin(),
                       _late_update_callbacks.end(), handle);
  if (pos != _late_update_callbacks.end()) _late_update_callbacks.erase(pos);

}
