#pragma once
#include <imgui/misc/imgui_stdlib.h>

#include <glm/glm.hpp>
#include <ui_drawable.hpp>

namespace nell::comp
{
struct FreeflightController final : UiDrawable
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

  void drawImGui () override;
  float fly_speed;
  float sensitivity;

  glm::vec3 euler_angles;

  bool is_rotating;
  bool invert_pitch;
  bool invert_yaw;
  double prev_xpos;
  double prev_ypos;
};

inline void FreeflightController::drawImGui ()
{
  if (ImGui::TreeNode("Freeflight Controller"))
  {
    ImGui::SliderFloat("Speed", &fly_speed, 0.1f, 20.0f);
    ImGui::SliderFloat("Sensitivity", &sensitivity, 0.001f,
                       0.1f);
    ImGui::Checkbox("Invert Pitch", &invert_pitch);
    ImGui::Checkbox("Invert Yaw", &invert_yaw);
    ImGui::TreePop();
  }
}

}  // namespace nell::comp
