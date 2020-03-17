#pragma once
#include <entt/entity/fwd.hpp>

namespace nell::comp
{
struct ChildLink
{
  std::vector<entt::entity> children;
};

struct ParentLink
{
  entt::entity parent;
};

}
