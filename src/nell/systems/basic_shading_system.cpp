#include <nell/systems/basic_shading_system.hpp>

namespace nell::systems
{
BasicShadingSystem::BasicShadingSystem() {}
BasicShadingSystem::~BasicShadingSystem () {}

void BasicShadingSystem::update (const double &time, const double &delta_time,
  entt::registry &reg) {}

void BasicShadingSystem::render (const double &time, const double &delta_time,
  entt::registry &reg) {}

}  // namespace nell::systems