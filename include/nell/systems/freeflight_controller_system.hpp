#pragma once
#include <entt/entity/registry.hpp>
#include <nell/input.hpp>

namespace nell::systems
{
void updateFreeflightController(entt::registry &, entt::entity &,
                                const input::NellInputList &, double);
void updateFreeflightControllers(entt::registry &, const input::NellInputList &,
                                 double);
}  // namespace nell::systems
