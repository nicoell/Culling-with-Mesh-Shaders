#pragma once
#include <imgui/misc/imgui_stdlib.h>

#include <glm/glm.hpp>

namespace nell::comp
{
struct FreeflightController
{
  FreeflightController()
      : fly_speed(1.f),
        sensitivity(0.01f),
        euler_angles(0),
        is_rotating(false),
        invert_pitch(false),
        invert_yaw(false),
        prev_xpos(0),
        prev_ypos(0)
  {
  }

  float fly_speed;
  float sensitivity;

  glm::vec3 euler_angles;

  bool is_rotating;
  bool invert_pitch;
  bool invert_yaw;
  double prev_xpos;
  double prev_ypos;
};

inline void drawComponentImpl(FreeflightController &freeflight_camera)
{
  if (ImGui::TreeNode("Freeflight Controller"))
  {
    ImGui::SliderFloat("Speed", &freeflight_camera.fly_speed, 0.1f, 20.0f);
    ImGui::SliderFloat("Sensitivity", &freeflight_camera.sensitivity, 0.001f,
                       0.1f);
    ImGui::Checkbox("Invert Pitch", &freeflight_camera.invert_pitch);
    ImGui::Checkbox("Invert Yaw", &freeflight_camera.invert_yaw);
    ImGui::TreePop();
  }
}
}  // namespace nell::comp
