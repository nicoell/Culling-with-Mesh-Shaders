#include <systems/callbacks/early_update_emitter.hpp>

unsigned nell::EarlyUpdateEmitter::addEarlyUpdateCallback (
  EarlyUpdateCallback &callback)
{
  unsigned handle = (_early_update_callbacks.empty())
                        ? 0
                        : _early_update_callbacks.back().handle + 1;

  _early_update_callbacks.emplace_back(handle, callback);
  return handle;
}

void nell::EarlyUpdateEmitter::removeEarlyUpdateCallback (unsigned handle) {
  auto pos = std::find(_early_update_callbacks.begin(),
                       _early_update_callbacks.end(), handle);
  if (pos != _early_update_callbacks.end()) _early_update_callbacks.erase(pos);

}
