#pragma once
#include <variant>

namespace input
{
struct NellInputKey
{
  int key;
  int scancode;
  int action;
  int mods;
};

struct NellInputMouseButton
{
  int button;
  int action;
  int mods;
};

struct NellInputCursorPos
{
  double xpos;
  double ypos;
};

struct NellInputScroll
{
  double xoffset;
  double yoffset;
};

using NellInput = std::variant<NellInputKey, NellInputMouseButton,
                               NellInputCursorPos, NellInputScroll>;
using NellInputList = std::vector<NellInput>;

// helper type for std::visit
template <class... Ts>
struct Overloaded : Ts...
{
  using Ts::operator()...;
};
template <class... Ts>
Overloaded(Ts...)->Overloaded<Ts...>;
}  // namespace input
