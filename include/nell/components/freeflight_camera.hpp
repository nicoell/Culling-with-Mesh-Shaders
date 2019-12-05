#pragma once
#include <glm/glm.hpp>

namespace nell::comp
{
struct FreeflightCamera
{
  float fly_speed;
  float sensitivity;

  bool is_rotating;
  bool is_valid;
  double prev_xpos;
  double prev_ypos;
};
}  // namespace nell::comp