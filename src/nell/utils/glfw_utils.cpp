#include <utils/glfw_utils.hpp>

namespace glfw_utils
{

void GlfwWindowDestroyHelper::operator() (GLFWwindow *ptr)
{
  glfwDestroyWindow(ptr);
}
}
