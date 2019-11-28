#pragma once
#include <GLFW/glfw3.h>

namespace utils
{
struct GlfwWindowDestroyHelper
{
  void operator()(GLFWwindow *ptr) { glfwDestroyWindow(ptr); }
};
}  // namespace utils