#include <fstream>
#include <nell/context.hpp>
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
  _window = glfwCreateWindow(_options.window_width, _options.window_height,
                             _options.window_title.c_str(), nullptr, nullptr);
  glfwSetWindowPos(_window, 100, 50);
  glfwSetWindowSizeCallback(_window, &Context::resizeCallback);
  glfwMakeContextCurrent(_window);

  const auto glad_load_status = gladLoadGL();
  if (!glad_load_status)
  {
    spdlog::critical("Error on OpenGL load with Glad {0:d}", glad_load_status);
    exit(-1);
  }

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
  //_scene = std::make_unique<Scene>();
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
      _scene->setTime(time);
      _scene->setDeltaTime(delta_time);
      _scene->update();
      _scene->render();
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // End Frame
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    endUiFrame();

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
void Context::resizeCallback(GLFWwindow*, int w, int h) {}

void Context::beginUiFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Context::loadScene(
    const std::pair<const std::string, SceneDefinitionFunction>& scene)
{
  if (_scene) _scene.reset();
  glfwSetWindowTitle(_window,
                     (_options.window_title + " : " + scene.first).c_str());
  _scene = std::make_unique<Scene>(scene.first, scene.second);
}

std::string Context::loadSceneArchiveFile(const std::string& archive_filename)
{
  const auto file = std::string(archive_filename + ".json");
  std::ifstream archive_ifstream(file);
  if (!archive_ifstream.is_open()) return std::string();
  return std::string{std::istreambuf_iterator{archive_ifstream}, {}};
}
bool Context::saveSceneArchiveFile(const std::string& archive_filename,
                                   const std::string& archive)
{
  const auto file = std::string(archive_filename + ".json");

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
    ImGui::InputTextWithHint("Settings Name", _scene->getArchiveFileName().c_str(), settings_name,
                             IM_ARRAYSIZE(settings_name));
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