#pragma once
#include <entt/entity/registry.hpp>

namespace nell::systems
{
  void updatePerspectiveCamera(entt::registry &, entt::entity &);
  void updatePerspectiveCameras(entt::registry &, entt::entity &);
}
