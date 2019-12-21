#include <fstream>
#include <nell/context.hpp>
#include <nell/definitions.hpp>
#include <utility>

namespace nell
{
Context::Context(ContextOptions creation_options)
    : _options(std::move(creation_options))
{
  spdlog::info("Create Context");
  spdlog::set_level(spdlog::level::debug);
  //  spdlog::flush_every(std::chrono::seconds(3));

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Init GLFW and OpenGL with GLAD
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  glfwSetErrorCallback(glfwErrorCallback);
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  _window = glfwCreateWindow(_options.window_width, _options.window_height,
                             _options.window_title.c_str(), nullptr, nullptr);
  glfwSetWindowPos(_window, 100, 50);
  glfwSetWindowUserPointer(_window, this);
  glfwSetWindowSizeCallback(_window, resizeCallback);
  glfwSetKeyCallback(_window, keyCallback);
  glfwSetMouseButtonCallback(_window, mouseButtonCallback);
  glfwSetScrollCallback(_window, scrollCallback);
  glfwMakeContextCurrent(_window);

  const auto glad_load_status = gladLoadGL();
  if (!glad_load_status)
  {
    spdlog::critical("Error on OpenGL load with Glad {0:d}", glad_load_status);
    exit(-1);
  }

  glDebugMessageCallback(logGlDebugMessage, nullptr);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Init ImGui
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  //_io = ImGui::GetIO();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(_window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Init Framework specifics: Scene
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
}

void Context::run()
{
  spdlog::info("Run");
  auto time = glfwGetTime();
  while (!glfwWindowShouldClose(_window))
  {
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Begin Frame
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const double delta_time = glfwGetTime() - time;
    time = glfwGetTime();
    beginUiFrame();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Update Frame
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    updateUiFrame();
    if (_scene)
    {
      _scene->update(time, delta_time, _input_list);
      _scene->render(time, delta_time);
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // End Frame
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    endUiFrame();
    _input_list.clear();

    glfwSwapBuffers(_window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(_window);
  glfwTerminate();
}

void Context::glfwErrorCallback(int error, const char* description)
{
  spdlog::error("glfw Error #{0:d}: {1}", error, description);
}

void Context::logGlDebugMessage(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* param)
{
  spdlog::level::level_enum loglevel;
  switch (severity)
  {
    case GL_DEBUG_SEVERITY_HIGH:
      loglevel = spdlog::level::critical;
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      loglevel = spdlog::level::err;
      break;
    case GL_DEBUG_SEVERITY_LOW:
      loglevel = spdlog::level::warn;
      break;
    default:
      loglevel = spdlog::level::info;
      break;
  }

  spdlog::log(
      loglevel, "[#{0:d}{1}] {2}; from {3}", id,
      [&]() -> std::string {
        switch (type)
        {
          case GL_DEBUG_TYPE_ERROR:
            return "GL_DEBUG_TYPE_ERROR";
          case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
          case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
          case GL_DEBUG_TYPE_PORTABILITY:
            return "GL_DEBUG_TYPE_PORTABILITY";
          case GL_DEBUG_TYPE_PERFORMANCE:
            return "GL_DEBUG_TYPE_PERFORMANCE";
          case GL_DEBUG_TYPE_MARKER:
            return "GL_DEBUG_TYPE_MARKER";
          case GL_DEBUG_TYPE_PUSH_GROUP:
            return "GL_DEBUG_TYPE_PUSH_GROUP";
          case GL_DEBUG_TYPE_POP_GROUP:
            return "GL_DEBUG_TYPE_POP_GROUP";
          case GL_DEBUG_TYPE_OTHER:
            return "GL_DEBUG_TYPE_OTHER";

          default:
            return std::to_string(type);
        }
      }(),
      message,
      [&]() -> std::string {
        switch (source)
        {
          case GL_DEBUG_SOURCE_API:
            return "GL_DEBUG_TYPE_ERROR";
          case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
          case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "GL_DEBUG_SOURCE_THIRD_PARTY";
          case GL_DEBUG_SOURCE_APPLICATION:
            return "GL_DEBUG_SOURCE_APPLICATION";
          default:
            return std::to_string(source);
        }
      }());
}

void Context::resizeCallback(GLFWwindow* window, int w, int h)
{
  static_cast<Context*>(glfwGetWindowUserPointer(window))->_scene->resize(w, h);
  glViewport(0, 0, w, h);
}

void Context::keyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods)
{
  static_cast<Context*>(glfwGetWindowUserPointer(window))
      ->_input_list.emplace_back<input::NellInputKey>(
          {key, scancode, action, mods});
}

void Context::mouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods)
{
  static_cast<Context*>(glfwGetWindowUserPointer(window))
      ->_input_list.emplace_back<input::NellInputMouseButton>(
          {button, action, mods});
}

void Context::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  static_cast<Context*>(glfwGetWindowUserPointer(window))
      ->_input_list.emplace_back<input::NellInputScroll>(
          {xoffset, yoffset});
}

void Context::beginUiFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Context::loadScene(
    const std::pair<const std::string, std::function<SceneImpl*()>> scene_impl)
{
  if (_scene) _scene.reset();
  glfwSetWindowTitle(
      _window, (_options.window_title + " : " + scene_impl.first).c_str());
  _scene = std::make_unique<Scene>(
      scene_impl.first, std::unique_ptr<SceneImpl>(scene_impl.second()));
}

std::string Context::loadSceneArchiveFile(const std::string& archive_filename)
{
  const auto file = std::string(kSettingsPath + archive_filename + ".json");
  std::ifstream archive_ifstream(file);
  if (!archive_ifstream.is_open()) return std::string();
  return std::string{std::istreambuf_iterator{archive_ifstream}, {}};
}
bool Context::saveSceneArchiveFile(const std::string& archive_filename,
                                   const std::string& archive)
{
  const auto file = std::string(kSettingsPath + archive_filename + ".json");

  std::ofstream archive_ofstream(file, std::ios::out | std::ios::trunc);
  if (!archive_ofstream.is_open()) return false;
  archive_ofstream.write(archive.data(), archive.size());
  return true;
}

void Context::updateUiFrame()
{
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Main Menu Bar
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  bool showSaveSettingsPopup = false;
  bool showLoadSettingsPopup = false;
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("Menu"))
    {
      if (ImGui::BeginMenu("Open Scene"))
      {
        for (const auto& scene_name_func_pair : _scene_definition_functions)
        {
          if (ImGui::MenuItem(scene_name_func_pair.first.c_str(), 0, false,
                              !(_scene && scene_name_func_pair.first ==
                                              _scene->getActiveScene())))
          {
            loadScene(scene_name_func_pair);
          }
        }
        ImGui::EndMenu();
      }
      if (ImGui::MenuItem("Reload Scene", 0, false, _scene.operator bool()))
      {
        const auto archive = _scene->serialize();
        _scene->deserialize(archive);
      }
      if (ImGui::MenuItem("Save Settings", 0, false, _scene.operator bool()))
      {
        const auto archive = _scene->serialize();
        saveSceneArchiveFile(_scene->getArchiveFileName(), archive);
      }
      if (ImGui::MenuItem("Save Settings As", 0, false, _scene.operator bool()))
      {
        showSaveSettingsPopup = true;
      }
      if (ImGui::MenuItem("Load Settings", 0, false, _scene.operator bool()))
      {
        showLoadSettingsPopup = true;
      }
      if (ImGui::MenuItem("Close Scene", 0, false, _scene.operator bool()))
      {
        if (_scene) _scene.reset();
        glfwSetWindowTitle(_window, _options.window_title.c_str());
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Save Settings As Popup
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (showSaveSettingsPopup) ImGui::OpenPopup("Save Settings As");
  if (ImGui::BeginPopupModal("Save Settings As"))
  {
    static char settings_name[128] = "";
    ImGui::InputTextWithHint("Settings Name", "DefaultSettings", settings_name,
                             IM_ARRAYSIZE(settings_name));
    if (ImGui::Button("Save"))
    {
      _scene->setArchiveName(std::string(settings_name));
      const auto archive = _scene->serialize();
      saveSceneArchiveFile(_scene->getArchiveFileName(), archive);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Abort"))
    {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Load Settings Popup
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (showLoadSettingsPopup) ImGui::OpenPopup("Load Settings");
  if (ImGui::BeginPopupModal("Load Settings"))
  {
    static char settings_name[128] = "";
    ImGui::InputTextWithHint("Settings Name",
                             _scene->getArchiveFileName().c_str(),
                             settings_name, IM_ARRAYSIZE(settings_name));
    if (ImGui::Button("Load"))
    {
      _scene->setArchiveName(std::string(settings_name));
      const auto archive = loadSceneArchiveFile(_scene->getArchiveFileName());
      _scene->deserialize(archive);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Abort"))
    {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

}  // namespace nell

void Context::endUiFrame()
{
  ImGui::Render();
  glViewport(0, 0, _options.window_width, _options.window_height);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
}  // namespace nell