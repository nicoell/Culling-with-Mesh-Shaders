#include <systems/callbacks/fixed_update_emitter.hpp>

unsigned nell::FixedUpdateEmitter::addFixedUpdateCallback (
  FixedUpdateCallback &callback)
{
  unsigned handle = (_fixed_update_callbacks.empty())
                        ? 0
                        : _fixed_update_callbacks.back().handle + 1;

  _fixed_update_callbacks.emplace_back(handle, callback);
  return handle;
}

void nell::FixedUpdateEmitter::removeFixedUpdateCallback (unsigned handle) {
  auto pos = std::find(_fixed_update_callbacks.begin(),
                       _fixed_update_callbacks.end(), handle);
  if (pos != _fixed_update_callbacks.end()) _fixed_update_callbacks.erase(pos);

}
