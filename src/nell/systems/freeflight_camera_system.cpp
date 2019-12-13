#include <GLFW/glfw3.h>
#include <freeflight_camera.hpp>
#include <nell/systems/freeflight_camera_system.hpp>
#include <transform.hpp>

void nell::systems::updateFreeflightCamera(entt::registry &reg,
                                           entt::entity &camera_entity,
                                           const input::NellInputList &input_list,
                                           double delta_time)
{
  auto [transform, freeflight_camera] =
      reg.get<comp::Transform, comp::FreeflightCamera>(camera_entity);

  for (auto &input : input_list)
  {
    std::visit(
        input::Overloaded{
            [&](const input::NellInputKey &arg) {
              if (arg.action != GLFW_PRESS) return;
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
                  transform.setTranslation(transform.getTranslation() +
                                           freeflight_camera.fly_speed *
                                               transform.getRight());
                  break;
                case GLFW_KEY_A:
                  transform.setTranslation(transform.getTranslation() -
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
              if (freeflight_camera.is_rotating && freeflight_camera.is_valid)
              {
                double delta_x = (arg.xpos - freeflight_camera.prev_xpos) *
                                 freeflight_camera.sensitivity;
                double delta_y = (arg.ypos - freeflight_camera.prev_ypos) *
                                 freeflight_camera.sensitivity;

                glm::vec3 angles = glm::eulerAngles(transform.getRotation());
                angles.x = angles.x + glm::radians(delta_x);
                angles.y = glm::radians(
                    glm::clamp(glm::degrees(angles.y) + delta_y, -89., 89.));

                transform.setRotation(glm::quat(angles));
              }
              freeflight_camera.is_valid = true;
              freeflight_camera.prev_xpos = arg.xpos;
              freeflight_camera.prev_ypos = arg.ypos;
            },
        },
        input);
  }
}

void nell::systems::updateFreeflightCameras(entt::registry &reg,
                                            const input::NellInputList &input_list,
                                            double delta_time)
{
}
