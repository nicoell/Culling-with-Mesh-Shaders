#pragma once
#include <entt/entity/registry.hpp>
#include <input.hpp>

namespace nell::systems
{
void updateFreeflightCamera(entt::registry &, entt::entity &,
                        input::NellInputList &, double);
void updateFreeflightCameras(entt::registry &, input::NellInputList &,
                             double);
}  // namespace nell::systems
