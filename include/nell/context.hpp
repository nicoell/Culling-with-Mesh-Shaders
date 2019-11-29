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
#include <basic_scene.hpp>
#include <map>
#include <nell/scene.hpp>

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
  inline static std::map<std::string, SceneDefinitionFunction>
      _scene_definition_functions = {{"Basic Scene", populateBasicScene},
                                     {"Test Scene", populateBasicScene}};
  static void glfwErrorCallback(int error, const char *description);
  static void resizeCallback(GLFWwindow *, int w, int h);

  void beginUiFrame();
  void loadScene(
      const std::pair<const std::string, const SceneDefinitionFunction> &scene);
  std::string loadSceneArchiveFile(const std::string &);
  bool saveSceneArchiveFile(const std::string &, const std::string &);
  void updateUiFrame();
  void endUiFrame();

  GLFWwindow *_window;
  std::unique_ptr<Scene> _scene;
  const ContextOptions _options;
};

}  // namespace nell
