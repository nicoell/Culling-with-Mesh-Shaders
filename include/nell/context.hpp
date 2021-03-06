// ----------------------------------------------------------------------------------
//  Culling with Mesh Shaders
//  File: context.hpp
//  Author: Nico Ell
//  Email: cvnicoell@gmail.com
//
//  Copyright (c) 2019, Nico. All rights reserved.
// ----------------------------------------------------------------------------------

#pragma once

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/impl/imgui_impl_glfw.h>
#include <imgui/impl/imgui_impl_opengl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <map>
#include <nell/input.hpp>
#include <nell/scene.hpp>
#include <nell/scenes/basic_scene.hpp>
#include <nell/scenes/culling_cs_scene.hpp>
#include <nell/scenes/culling_mesh_scene.hpp>
#include <nell/scenes/mesh_shader_scene.hpp>
#include <nell/scenes/scene_impl.hpp>
#include <nell/scenes/task_mesh_shader_scene.hpp>

namespace nell
{
struct ContextOptions
{
  int window_width;
  int window_height;
  std::string window_title;

  std::string scene_id;
};

class Context
{
 public:
  explicit Context(ContextOptions creation_options);
  void run();

 private:
  inline static std::map<std::string, std::function<SceneImpl *()>>
      _scene_definition_functions = {
          {"Basic Scene", []() { return new BasicScene(); }},
          {"Basic MeshShader Scene", []() { return new MeshShaderScene(); }},
          {"Basic TaskMeshShader Scene",
           []() { return new TaskMeshShaderScene(); }},
          {"Culling TaskMesh Scene", []() { return new CullingMeshScene(); }},
          {"Culling Compute Scene", []() { return new CullingCsScene(); }}};
  static void glfwErrorCallback(int error, const char *description);

  static void logGlDebugMessage(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *param);

  static void resizeCallback(GLFWwindow *, int w, int h);
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);
  static void mouseButtonCallback(GLFWwindow *window, int button, int action,
                                  int mods);
  static void scrollCallback(GLFWwindow *window, double xoffset,
                             double yoffset);
  static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

  void beginUiFrame();
  void loadScene(
      const std::pair<const std::string, std::function<SceneImpl *()>> &);
  std::string loadSceneArchiveFile(const std::string &);
  bool saveSceneArchiveFile(const std::string &, const std::string &);
  void updateUiFrame();
  void endUiFrame();

  GLFWwindow *_window;
  std::unique_ptr<Scene> _scene;
  input::NellInputList _input_list;
  const ContextOptions _options;
  int _width;
  int _height;

  double _frame_start_time;
  double _delta_time;

  gl_utils::ParameterQueries _parameter_queries;
};

}  // namespace nell
