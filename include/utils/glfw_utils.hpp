#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace glfw_utils
{
struct GlfwWindowDestroyHelper
{
  void operator()(GLFWwindow *ptr);
};
}  // namespace glfw_utils
