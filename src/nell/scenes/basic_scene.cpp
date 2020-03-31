#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nell/components/transform.hpp>
#include <nell/components/triangle_mesh.hpp>
#include <nell/components/triangle_mesh_draw_elements_objects.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/basic_scene.hpp>
#include <nell/systems/model_import_system.hpp>
#include <utils/gl_utils.hpp>

static constexpr unsigned int kPipelineCount = 1;
static constexpr GLuint kRenderPipeline = 0;

static constexpr auto kAttrPosition = "position";
static constexpr auto kAttrNormal = "normal";
static constexpr auto kUniformMvp = "mvp";
static constexpr auto kUniformModelMatrixTransposeInverse =
    "model_matrix_transpose_inverse";

static constexpr auto kVerticesBindingIndex = 0;
static constexpr auto kNormalBindingIndex = 1;

namespace nell
{
BasicScene::BasicScene()
    : _basic_vert_shader(
          0, GL_VERTEX_SHADER,
          std::string(kShaderPath) + std::string("basic_shader.vert")),
      _basic_fragment_shader(
          0, GL_FRAGMENT_SHADER,
          std::string(kShaderPath) + std::string("basic_shader.frag"))
{
  glCreateProgramPipelines(kPipelineCount, _program_pipeline);
  glUseProgramStages(_program_pipeline[kRenderPipeline], GL_VERTEX_SHADER_BIT,
                     static_cast<GLuint>(_basic_vert_shader));
  glUseProgramStages(_program_pipeline[kRenderPipeline], GL_FRAGMENT_SHADER_BIT,
                     static_cast<GLuint>(_basic_fragment_shader));

  _position_attr_loc =
      _basic_vert_shader.getProgramInput(kAttrPosition).location;
  _normal_attr_loc = _basic_vert_shader.getProgramInput(kAttrNormal).location;

  _uniform_mvp_loc = _basic_vert_shader.getUniform(kUniformMvp).location;
  _uniform_model_matrix_transpose_inverse =
      _basic_vert_shader.getUniform(kUniformModelMatrixTransposeInverse)
          .location;
}

void BasicScene::populate(Scene *scene, entt::registry &reg)
{
  {
    auto entity = reg.create();
    reg.assign<comp::EntityName>(entity, "Armadillo");
    auto &asp = reg.assign<comp::ModelSource>(entity);
    auto &tf = reg.assign<comp::Transform>(entity);
    asp.path = "Armadillo.ply";
  }
  /*{
    auto entity = reg.create();
    reg.assign<comp::EntityName>(entity, "Thai Statue");
    auto &asp = reg.assign<comp::ModelSource>(entity);
    auto &tf = reg.assign<comp::Transform>(entity);
    asp.path = "xyzrgb_statuette.ply";
    // Get model from: http://graphics.stanford.edu/data/3Dscanrep/
  }*/
  /*{
    auto entity = reg.create();
    reg.assign<comp::EntityName>(entity, "Sponza");
    auto &asp = reg.assign<comp::ModelSource>(entity);
    auto &tf = reg.assign<comp::Transform>(entity);
    asp.path = "sponza.obj";
  }*/

  systems::importModelsFromSource<comp::TriangleMesh>(reg);
}

void BasicScene::setup(Scene *scene, entt::registry &reg)
{
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ### Setup VAO and Mesh Buffers ###
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Setup VAO
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  glCreateVertexArrays(1, &_vertex_array_object);
  // Enable vao attributes
  glEnableVertexArrayAttrib(_vertex_array_object, _position_attr_loc);
  glEnableVertexArrayAttrib(_vertex_array_object, _normal_attr_loc);
  // Format vao attributes
  glVertexArrayAttribFormat(_vertex_array_object, _position_attr_loc,
                            comp::TriangleMesh::getVertexSize(),
                            comp::TriangleMesh::getVertexType(), GL_FALSE, 0);
  glVertexArrayAttribFormat(_vertex_array_object, _normal_attr_loc,
                            comp::TriangleMesh::getNormalSize(),
                            comp::TriangleMesh::getNormalType(), GL_FALSE, 0);

  // Set binding location of attributes
  glVertexArrayAttribBinding(_vertex_array_object, _position_attr_loc,
                             kVerticesBindingIndex);
  glVertexArrayAttribBinding(_vertex_array_object, _normal_attr_loc,
                             kNormalBindingIndex);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto mesh_view = reg.view<comp::TriangleMesh>();

  for (auto entity : mesh_view)
  {
    auto &triangle_mesh = mesh_view.get<comp::TriangleMesh>(entity);
    auto &draw_elements_buffer =
        reg.assign<comp::TriangleMeshDrawElementsObjects>(entity);

    auto &vbo = draw_elements_buffer.vertex_buffer_object;
    auto &nbo = draw_elements_buffer.normal_buffer_object;
    auto &ibo = draw_elements_buffer.index_buffer_object;

    // Setup VBO
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, triangle_mesh.getVerticesSize(),
                         triangle_mesh.vertices.data(), 0);
    // Setup NBO
    glCreateBuffers(1, &nbo);
    glNamedBufferStorage(nbo, triangle_mesh.getNormalsSize(),
                         triangle_mesh.normals.data(), 0);

    // Setup IBO
    glCreateBuffers(1, &ibo);
    glNamedBufferStorage(ibo, triangle_mesh.getIndicesSize(),
                         triangle_mesh.indices.data(), 0);
  }

  // Timer Queries
  glGenQueries(2, _gpu_timer_query);
  // Dummy query
  glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.pong()]);
  glEndQuery(GL_TIME_ELAPSED);
}
void BasicScene::resize(int w, int h) {}

void BasicScene::update(Scene *scene, entt ::registry &reg,
                        const input::NellInputList &input_list,
                        const double &time, const double &delta_time)
{
  scene->drawComponentImGui<comp::TriangleMesh,
                            comp::TriangleMeshDrawElementsObjects>();
}
void BasicScene::render(Scene *scene, entt::registry &reg,
                        entt::entity &camera_entity, const double &time,
                        const double &delta_time)
{
  _cpu_time = glfwGetTime();
  glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.ping()]);

  auto &perspective_camera = reg.get<comp::PerspectiveCamera>(camera_entity);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ### Render Meshes with Basic Shader ###
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Get Entities with Model of BasisShadingMesh and iterate
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto render_objects =
      reg.view<comp::TriangleMesh, comp::TriangleMeshDrawElementsObjects,
               comp::Transform>();
  const auto view_proj_matrix = perspective_camera.getViewProjectionMatrix();
  const auto vertex_size_t = comp::TriangleMesh::getVertexSizeT();
  const auto normal_size_t = comp::TriangleMesh::getNormalSizeT();

  _rendered_objects = 0;

  glUseProgram(0);  // Unbind any bind programs to use program pipeline instead.

  for (auto entity : render_objects)
  {
    _rendered_objects++;

    auto [triangle_mesh, gpu_buffers, transform] =
        render_objects
            .get<comp::TriangleMesh, comp::TriangleMeshDrawElementsObjects,
                 comp::Transform>(entity);

    auto model_matrix = transform.getTransformation();
    auto model_matrix_transpose_inverse = glm::inverseTranspose(model_matrix);

    auto mvp_matrix = view_proj_matrix * model_matrix;

    glBindProgramPipeline(_program_pipeline[kRenderPipeline]);

    auto &vbo = gpu_buffers.vertex_buffer_object;
    auto &nbo = gpu_buffers.normal_buffer_object;
    auto &ibo = gpu_buffers.index_buffer_object;

    // Bind vbo to vao at bindloc
    glVertexArrayVertexBuffer(_vertex_array_object, kVerticesBindingIndex, vbo,
                              0, vertex_size_t);
    // Bind nbo to vao at bindloc
    glVertexArrayVertexBuffer(_vertex_array_object, kNormalBindingIndex, nbo, 0,
                              normal_size_t);
    // Bind indexbuffer to vao
    glVertexArrayElementBuffer(_vertex_array_object, ibo);

    glProgramUniformMatrix4fv(static_cast<GLuint>(_basic_vert_shader),
                              _uniform_mvp_loc, 1, GL_FALSE,
                              glm::value_ptr(mvp_matrix));
    glProgramUniformMatrix4fv(static_cast<GLuint>(_basic_vert_shader),
                              _uniform_model_matrix_transpose_inverse, 1,
                              GL_FALSE,
                              glm::value_ptr(model_matrix_transpose_inverse));

    glBindVertexArray(_vertex_array_object);
    // Draw Call
    glDrawElements(GL_TRIANGLES, triangle_mesh.indices.size(), GL_UNSIGNED_INT,
                   nullptr);
  }

  glEndQuery(GL_TIME_ELAPSED);

  _cpu_time = (glfwGetTime() - _cpu_time) * 1000.0;
  glGetQueryObjectui64v(_gpu_timer_query[_swap_id.pong()], GL_QUERY_RESULT,
                        &_gpu_time);

  _swap_id.swap();
}

void BasicScene::drawSceneImGui(Scene *scene, entt::registry &reg) {}

void BasicScene::drawStatsImGui(Scene *scene, entt::registry &reg,
                                const double &time, const double &delta_time)
{
  constexpr size_t res = 32;

  const double refresh_interval_min = 0., refresh_interval_max = 5;

  static bool draw_time_plot = true;
  static double refresh_interval = 4.0;
  static bool show_gpu_ms = false;
  static std::string cpu_histogram_text;
  static std::string gpu_histogram_text;
  static float cpu_time_data[res] = {0.0f};
  static float gpu_time_data[res] = {0.0f};
  static float cpu_histogram_max = FLT_MAX;
  static float gpu_histogram_max = FLT_MAX;

  double gpu_time_ms = _gpu_time * 1e-6;
  _cpu_time_min = glm::min(_cpu_time_min, _cpu_time);
  _cpu_time_max = glm::max(_cpu_time_max, _cpu_time);
  _gpu_time_min = glm::min(_gpu_time_min, _gpu_time);
  _gpu_time_max = glm::max(_gpu_time_max, _gpu_time);

  _frame_count++;
  _time_acc += delta_time;

  _cpu_time_acc += _cpu_time;
  _gpu_time_acc += _gpu_time;

  if (_time_acc > refresh_interval)
  {
    // Update CPU Timing Data
    _cpu_time_avg = _cpu_time_acc / _frame_count;
    if (draw_time_plot)
    {
      memmove(&cpu_time_data[0], &cpu_time_data[1], (res - 1) * sizeof(float));
      cpu_time_data[res - 1] = static_cast<float>(_cpu_time_avg);
    }
    _cpu_time_acc = 0;
    // Update GPU Timing Data
    _gpu_time_avg = _gpu_time_acc / _frame_count;
    if (draw_time_plot)
    {
      memmove(&gpu_time_data[0], &gpu_time_data[1], (res - 1) * sizeof(float));
      gpu_time_data[res - 1] = static_cast<float>(_gpu_time_avg * 1e-6);
    }
    _gpu_time_acc = 0;

    _time_acc = 0;
    _frame_count = 0;
  }

  ImGui::InputInt("Rendered Objects:", &_rendered_objects, 0, 0,
                  ImGuiInputTextFlags_ReadOnly);
  ImGui::Checkbox("Draw Time plots", &draw_time_plot);
  ImGui::SliderScalar("Refresh Interval", ImGuiDataType_Double,
                      &refresh_interval, &refresh_interval_min,
                      &refresh_interval_max, "%.1f s");

  ImGui::InputDouble("CPU Time (ms)", &_cpu_time, 0, 0, "%.2f",
                     ImGuiInputTextFlags_ReadOnly);
  ImGui::InputDouble("CPU Time Min (ms)", &_cpu_time_min, 0, 0, "%.2f",
                     ImGuiInputTextFlags_ReadOnly);
  ImGui::InputDouble("CPU Time Max (ms)", &_cpu_time_max, 0, 0, "%.2f",
                     ImGuiInputTextFlags_ReadOnly);
  ImGui::InputDouble("CPU Time Avg (ms)", &_cpu_time_avg, 0, 0, "%.2f",
                     ImGuiInputTextFlags_ReadOnly);
  if (draw_time_plot)
  {
    ImGui::InputFloat("CPU Histogram Max", &cpu_histogram_max);
    if (cpu_histogram_max < 0) cpu_histogram_max = FLT_MAX;
    ImGui::Text("Avg CPU Time %.4f within %.1fs", _cpu_time_avg,
                refresh_interval);
    ImGui::PlotHistogram("CPU Times", cpu_time_data,
                         IM_ARRAYSIZE(cpu_time_data), 0, nullptr, 0,
                         cpu_histogram_max,
                         ImVec2(0, 80));
  }

  ImGui::Checkbox("Show GPU Time in ms", &show_gpu_ms);
  if (show_gpu_ms)
  {
    double gpu_time_min_ms = _gpu_time_min * 1e-6;
    double gpu_time_max_ms = _gpu_time_max * 1e-6;
    double gpu_time_avg_ms = _gpu_time_avg * 1e-6;
    ImGui::InputDouble("GPU Time (ms)", &gpu_time_ms, 0, 0, "%.2f",
                       ImGuiInputTextFlags_ReadOnly);
    ImGui::InputDouble("GPU Time Min (ms)", &gpu_time_min_ms, 0, 0, "%.2f",
                       ImGuiInputTextFlags_ReadOnly);
    ImGui::InputDouble("GPU Time Max (ms)", &gpu_time_max_ms, 0, 0, "%.2f",
                       ImGuiInputTextFlags_ReadOnly);
    ImGui::InputDouble("GPU Time Avg (ms)", &gpu_time_avg_ms, 0, 0, "%.2f",
                       ImGuiInputTextFlags_ReadOnly);
  } else
  {
    ImGui::InputScalar("GPU Time (ns)", ImGuiDataType_U64, &_gpu_time, 0, 0, 0,
                       ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("GPU Time Min (ns)", ImGuiDataType_U64, &_gpu_time_min,
                       0, 0, 0, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("GPU Time Max (ns)", ImGuiDataType_U64, &_gpu_time_max,
                       0, 0, 0, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("GPU Time Avg (ns)", ImGuiDataType_U64, &_gpu_time_avg,
                       0, 0, 0, ImGuiInputTextFlags_ReadOnly);
  }

  if (draw_time_plot)
  {
    ImGui::InputFloat("GPU Histogram Max", &gpu_histogram_max);
    if (gpu_histogram_max < 0) gpu_histogram_max = FLT_MAX;
    ImGui::Text("Avg GPU Time %.4f within %.1fs", _gpu_time_avg * 1e-6,
                refresh_interval);
    ImGui::PlotHistogram("GPU Times", gpu_time_data,
                         IM_ARRAYSIZE(gpu_time_data), 0, nullptr, 0,
                         gpu_histogram_max,
                         ImVec2(0, 80));
  }
}
}  // namespace nell
