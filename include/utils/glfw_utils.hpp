#pragma once
#include <GLFW/glfw3.h>

namespace glfw_utils
{
struct GlfwWindowDestroyHelper
{
  void operator() (GLFWwindow *ptr);
};
}  // namespace utils