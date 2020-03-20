#include <nell/components/perspective_camera.hpp>
#include <nell/systems/camera_system.hpp>

namespace nell::systems
{
void updatePerspectiveCamera(entt::registry &reg, entt::entity &camera_entity)
{
  // TODO: Introduce Uniform Buffer with camera related stuff, manage in a
  // system
  auto [transform, perspective_camera] =
      reg.get<comp::Transform, comp::PerspectiveCamera>(camera_entity);

  perspective_camera.updateMatrices(transform);
}
}  // namespace nell::systems
