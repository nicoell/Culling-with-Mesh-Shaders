#pragma once
#include <entt/entity/registry.hpp>
#include <input.hpp>

namespace nell::systems
{
void updateFreeflightCamera(entt::registry &, entt::entity &,
                        const input::NellInputList &, double);
void updateFreeflightCameras(entt::registry &, const input::NellInputList &,
                             double);
}  // namespace nell::systems
