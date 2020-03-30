#pragma once

namespace utils
{
template <class T>
class PingPongId
{
 public:
  void swap() { _ping = pong(); }
  T ping() { return _ping; };
  T pong() { return (_ping + 1) % 2; }

 private:
  T _ping;
};
}  // namespace utils
