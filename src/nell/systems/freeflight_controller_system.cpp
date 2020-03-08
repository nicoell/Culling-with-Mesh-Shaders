#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>
#include <nell/components/freeflight_controller.hpp>
#include <nell/components/transform.hpp>
#include <nell/systems/freeflight_controller_system.hpp>

void nell::systems::updateFreeflightController(
    entt::registry &reg, entt::entity &camera_entity,
    const input::NellInputList &input_list, double delta_time)
{
  auto [transform, freeflight_camera] =
      reg.get<comp::Transform, comp::FreeflightController>(camera_entity);

  for (auto &input : input_list)
  {
    std::visit(input::Overloaded{
                   [&](const input::NellInputKey &arg) {
                     if (arg.action == GLFW_RELEASE) return;
                     switch (arg.key)
                     {
                       case GLFW_KEY_W:
                         transform.setTranslation(transform.getTranslation() +
                                                  freeflight_camera.fly_speed *
                                                      transform.getForward());
                         break;
                       case GLFW_KEY_S:
                         transform.setTranslation(transform.getTranslation() -
                                                  freeflight_camera.fly_speed *
                                                      transform.getForward());
                         break;
                       case GLFW_KEY_D:
                         transform.setTranslation(transform.getTranslation() -
                                                  freeflight_camera.fly_speed *
                                                      transform.getRight());
                         break;
                       case GLFW_KEY_A:
                         transform.setTranslation(transform.getTranslation() +
                                                  freeflight_camera.fly_speed *
                                                      transform.getRight());
                         break;
                       default:
                         break;
                     }
                   },
                   [&](const input::NellInputMouseButton &arg) {
                     if (arg.button == GLFW_MOUSE_BUTTON_RIGHT &&
                         arg.action == GLFW_PRESS)
                     {
                       freeflight_camera.is_rotating = true;
                     } else if (arg.button == GLFW_MOUSE_BUTTON_RIGHT &&
                                arg.action == GLFW_RELEASE)
                     {
                       freeflight_camera.is_rotating = false;
                     }
                   },
                   [&](const input::NellInputScroll &arg) {},
                   [&](const input::NellInputCursorPos &arg) {
                     if (freeflight_camera.is_rotating)
                     {
                       glm::vec3 delta_euler = {
                           (arg.ypos - freeflight_camera.prev_ypos) *
                               freeflight_camera.sensitivity,
                           (arg.xpos - freeflight_camera.prev_xpos) *
                               freeflight_camera.sensitivity,
                           0};  // Pitch, Yaw, Roll

                       constexpr float pitch_limit = glm::radians(89.0f);

                       glm::vec3 angles = freeflight_camera.euler_angles;

                       auto pitch_behaviour =
                           freeflight_camera.invert_pitch ? -1.0f : 1.0f;
                       auto yaw_behaviour =
                           freeflight_camera.invert_yaw ? 1.0f : -1.0f;

                       angles.x = glm::clamp(
                           angles.x + pitch_behaviour * delta_euler.x,
                           -pitch_limit, pitch_limit);
                       angles.y = angles.y + yaw_behaviour * delta_euler.y;

                       freeflight_camera.euler_angles = angles;

                       transform.setRotation(angles);
                     }

                     freeflight_camera.prev_xpos = arg.xpos;
                     freeflight_camera.prev_ypos = arg.ypos;
                   },
               },
               input);
  }
}

void nell::systems::updateFreeflightControllers(
    entt::registry &reg, const input::NellInputList &input_list,
    double delta_time)
{
  // TODO
}
