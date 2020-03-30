#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <nell/components/meshlet_triangle_mesh.hpp>
#include <nell/components/meshlet_triangle_mesh_objects.hpp>
#include <nell/components/transform.hpp>
#include <nell/components/triangle_mesh_draw_meshtasks_objects.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/culling_mesh_scene.hpp>
#include <nell/systems/model_import_system.hpp>
#include <utils/gl_utils.hpp>
#include <utils/glm_utils.hpp>

static constexpr unsigned int kPipelineCount = 1;
static constexpr GLuint kTestPipeline = 0;

static constexpr unsigned int kShaderProgramCount = 1;

static constexpr auto kUniformViewOrigin = "view_origin";
// static constexpr auto kUniformViewUp = "view_up";
// static constexpr auto kUniformViewRight = "view_right";
static constexpr auto kUniformDebugValue = "debug_value";
static constexpr auto kUniformFrustumPlanes = "frustum_planes[0]";
static constexpr auto kUniformDisableFrustumCulling = "disable_frustum_culling";
static constexpr auto kUniformDisableBackfaceCulling =
    "disable_backface_culling";

static constexpr auto kUniformMvp = "mvp";
static constexpr auto kUniformModelMatrixTransposeInverse =
    "model_matrix_transpose_inverse";

namespace nell
{
CullingMeshScene::CullingMeshScene()
    : _cpu_time(0),
      _cpu_time_min(DBL_MAX),
      _cpu_time_max(0),
      _cpu_time_avg(0),
      _gpu_time(0),
      _gpu_time_min(UINT_MAX),
      _gpu_time_max(0),
      _gpu_time_avg(0),
      _swap_id(),
      _disable_frustum_culling(false),
      _freeze_frustum_culling(false),
      _disable_backface_culling(false),
      _freeze_backface_culling(false),
      _culling_task_shader(
          0, GL_TASK_SHADER_NV,
          std::string(kShaderPath) + std::string("culling_shader.task")),
      _culling_mesh_shader(
          0, GL_MESH_SHADER_NV,
          std::string(kShaderPath) + std::string("culling_shader.mesh")),
      _culling_fragment_shader(
          0, GL_FRAGMENT_SHADER,
          std::string(kShaderPath) + std::string("culling_shader.frag"))
{
  _resource_location_maps.reserve(kShaderProgramCount);

  glCreateProgramPipelines(kPipelineCount, _program_pipeline);
  glUseProgramStages(_program_pipeline[kTestPipeline], GL_TASK_SHADER_BIT_NV,
                     static_cast<GLuint>(_culling_task_shader));
  glUseProgramStages(_program_pipeline[kTestPipeline], GL_MESH_SHADER_BIT_NV,
                     static_cast<GLuint>(_culling_mesh_shader));
  glUseProgramStages(_program_pipeline[kTestPipeline], GL_FRAGMENT_SHADER_BIT,
                     static_cast<GLuint>(_culling_fragment_shader));

  _uniform_view_origin_loc =
      _culling_task_shader.getUniform(kUniformViewOrigin).location;
  //_uniform_view_up_loc =
  //    _culling_task_shader.getUniform(kUniformViewUp).location;
  //_uniform_view_right_loc =
  //    _culling_task_shader.getUniform(kUniformViewRight).location;
  _uniform_frustum_planes_loc =
      _culling_task_shader.getUniform(kUniformFrustumPlanes).location;
  _uniform_debug_value_loc =
      _culling_task_shader.getUniform(kUniformDebugValue).location;
  _uniform_disable_frustum_culling_loc =
      _culling_task_shader.getUniform(kUniformDisableFrustumCulling).location;
  _uniform_disable_backface_culling_loc =
      _culling_task_shader.getUniform(kUniformDisableBackfaceCulling).location;

  _uniform_mvp_loc = _culling_mesh_shader.getUniform(kUniformMvp).location;
  _uniform_model_matrix_transpose_inverse =
      _culling_mesh_shader.getUniform(kUniformModelMatrixTransposeInverse)
          .location;
}

void CullingMeshScene::populate(Scene *scene, entt::registry &reg)
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
  }*/
  /*{
    auto entity = reg.create();
    reg.assign<comp::EntityName>(entity, "Sponza");
    auto &asp = reg.assign<comp::ModelSource>(entity);
    auto &tf = reg.assign<comp::Transform>(entity);
    asp.path = "sponza.obj";
  }*/

  systems::importModelsFromSource<comp::MeshletTriangleMesh>(reg);
}

void CullingMeshScene::setup(Scene *scene, entt::registry &reg)
{
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto mesh_view = reg.view<comp::MeshletTriangleMesh>();

  for (auto entity : mesh_view)
  {
    auto &meshlet_triangle_mesh =
        mesh_view.get<comp::MeshletTriangleMesh>(entity);
    auto &meshlet_triangle_mesh_buffers =
        reg.assign<comp::MeshletTriangleMeshObjects>(entity);

    auto &vdesc_ssbo = meshlet_triangle_mesh_buffers.vertex_descriptor_ssbo;
    auto &mdesc_ssbo = meshlet_triangle_mesh_buffers.meshlet_descriptor_ssbo;
    auto &i_ssbo = meshlet_triangle_mesh_buffers.index_ssbo;

    // Setup VSSBO
    glCreateBuffers(1, &vdesc_ssbo);
    glNamedBufferStorage(vdesc_ssbo,
                         meshlet_triangle_mesh.getVertexDescriptorSize(),
                         meshlet_triangle_mesh.vertex_descriptors.data(), 0);

    // Setup NSSBO
    glCreateBuffers(1, &mdesc_ssbo);
    glNamedBufferStorage(mdesc_ssbo,
                         meshlet_triangle_mesh.getMeshletDescriptorSize(),
                         meshlet_triangle_mesh.meshlet_descriptors.data(), 0);

    // Setup ISSBO
    glCreateBuffers(1, &i_ssbo);
    glNamedBufferStorage(i_ssbo, meshlet_triangle_mesh.getIndicesSize(),
                         meshlet_triangle_mesh.indices.data(), 0);
  }

  // Timer Queries
  glGenQueries(2, _gpu_timer_query);
  // Dummy query
  glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.pong()]);
  glEndQuery(GL_TIME_ELAPSED);
}
void CullingMeshScene::resize(int w, int h) {}

void CullingMeshScene::update(Scene *scene, entt ::registry &reg,
                              const input::NellInputList &input_list,
                              const double &time, const double &delta_time)
{
  scene->drawComponentImGui<comp::MeshletTriangleMesh,
                            comp::TriangleMeshDrawMeshtasksObjects>();
}
void CullingMeshScene::render(Scene *scene, entt::registry &reg,
                              entt::entity &camera_entity, const double &time,
                              const double &delta_time)
{
  _cpu_time = glfwGetTime();
  glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.ping()]);

  auto [perspective_camera, cam_transform] =
      reg.get<comp::PerspectiveCamera, comp::Transform>(camera_entity);
  auto view_proj_matrix = perspective_camera.getViewProjectionMatrix();
  auto view_matrix = perspective_camera.getViewMatrix();
  auto view_matrix_inverse = glm::inverse(perspective_camera.getViewMatrix());
  auto view_matrix_transpose_inverse = glm::transpose(view_matrix_inverse);
  auto frustum_planes = perspective_camera.getFrustumPlanes();
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ### Render Meshes with Basic Shader ###
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Get Entities with Model of BasisShadingMesh and iterate
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto render_objects =
      reg.view<comp::MeshletTriangleMesh, comp::MeshletTriangleMeshObjects,
               comp::Transform>();
  _rendered_objects = 0;

  glUseProgram(0);  // Unbind any bind programs to use program pipeline instead.

  for (auto entity : render_objects)
  {
    _rendered_objects++;

    auto [meshlet_triangle_mesh, gpu_buffers, transform] =
        render_objects.get<comp::MeshletTriangleMesh,
                           comp::MeshletTriangleMeshObjects, comp::Transform>(
            entity);

    glBindProgramPipeline(_program_pipeline[kTestPipeline]);

    auto &vdesc_ssbo = gpu_buffers.vertex_descriptor_ssbo;
    auto &mdesc_ssbo = gpu_buffers.meshlet_descriptor_ssbo;
    auto &i_ssbo = gpu_buffers.index_ssbo;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, i_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vdesc_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mdesc_ssbo);

    auto model_matrix = transform.getTransformation();
    auto model_matrix_inverse = glm::inverse(model_matrix);
    auto model_matrix_transpose_inverse = glm::transpose(model_matrix_inverse);

    auto mvp_matrix = view_proj_matrix * model_matrix;

    if (!_freeze_frustum_culling)
    {
      _cached_frustum_planes = glm_utils::getClippingPlanes(mvp_matrix);
    }

    if (!_freeze_backface_culling)
    {
      _cached_view_up = glm::normalize(model_matrix_inverse *
                                       glm::vec4(cam_transform.getUp(), 0));
      _cached_view_right = glm::normalize(
          model_matrix_inverse * glm::vec4(cam_transform.getRight(), 0));
      _cached_view_origin =
          model_matrix_inverse * glm::vec4(cam_transform.getTranslation(), 1);
    }

    glProgramUniformMatrix4fv(static_cast<GLuint>(_culling_mesh_shader),
                              _uniform_mvp_loc, 1, GL_FALSE,
                              glm::value_ptr(mvp_matrix));

    glProgramUniformMatrix4fv(static_cast<GLuint>(_culling_mesh_shader),
                              _uniform_model_matrix_transpose_inverse, 1,
                              GL_FALSE,
                              glm::value_ptr(model_matrix_transpose_inverse));

    glProgramUniform1f(static_cast<GLuint>(_culling_task_shader),
                       _uniform_debug_value_loc, 0.f);

    glProgramUniform1i(static_cast<GLuint>(_culling_task_shader),
                       _uniform_disable_frustum_culling_loc,
                       _disable_frustum_culling);

    glProgramUniform1i(static_cast<GLuint>(_culling_task_shader),
                       _uniform_disable_backface_culling_loc,
                       _disable_backface_culling);

    // glProgramUniform3fv(static_cast<GLuint>(_culling_task_shader),
    //                    _uniform_view_up_loc, 1,
    //                    glm::value_ptr(_cached_view_up));

    // glProgramUniform3fv(static_cast<GLuint>(_culling_task_shader),
    //                    _uniform_view_right_loc, 1,
    //                    glm::value_ptr(_cached_view_right));

    glProgramUniform3fv(static_cast<GLuint>(_culling_task_shader),
                        _uniform_view_origin_loc, 1,
                        glm::value_ptr(_cached_view_origin));

    glProgramUniform4fv(static_cast<GLuint>(_culling_task_shader),
                        _uniform_frustum_planes_loc, 6,
                        glm::value_ptr(_cached_frustum_planes[0]));

    GLuint task_count =
        (meshlet_triangle_mesh.meshlet_descriptors.size() / 32) +
        (meshlet_triangle_mesh.meshlet_descriptors.size() % 32 != 0);

    glDrawMeshTasksNV(0, task_count);
  }

  glEndQuery(GL_TIME_ELAPSED);
  glGetQueryObjectui64v(_gpu_timer_query[_swap_id.pong()], GL_QUERY_RESULT,
                        &_gpu_time);

  _cpu_time = (glfwGetTime() - _cpu_time) * 1000.0;
  _swap_id.swap();
}

void CullingMeshScene::drawSceneImGui(Scene *scene, entt::registry &reg)
{
  ImGui::Checkbox("Disable Frustum Culling", &_disable_frustum_culling);
  if (!_disable_frustum_culling)
  {
    ImGui::Checkbox("Freeze Frustum Culling", &_freeze_frustum_culling);
  }
  ImGui::Checkbox("Disable Backface Culling", &_disable_backface_culling);
  if (!_disable_backface_culling)
  {
    ImGui::Checkbox("Freeze Backface Culling", &_freeze_backface_culling);
  }
}

void CullingMeshScene::drawStatsImGui(Scene *scene, entt::registry &reg,
                                      const double &time,
                                      const double &delta_time)
{
  constexpr size_t res = 32;
  constexpr double cpu_interval = 100.0;
  constexpr GLuint gpu_interval = 1e+8;

  static bool draw_time_plot = true;
  static bool show_gpu_ms = false;
  static std::string cpu_histogram_text;
  static std::string gpu_histogram_text;
  static float cpu_time_data[res] = {0.0f};
  static float gpu_time_data[res] = {0.0f};

  double gpu_time_ms = _gpu_time * 1e-6;
  _cpu_time_min = glm::min(_cpu_time_min, _cpu_time);
  _cpu_time_max = glm::max(_cpu_time_max, _cpu_time);
  _gpu_time_min = glm::min(_gpu_time_min, _gpu_time);
  _gpu_time_max = glm::max(_gpu_time_max, _gpu_time);

  _cpu_time_acc += _cpu_time;
  _cpu_frame_count++;
  if (_cpu_time_acc > cpu_interval)
  {
    _cpu_time_avg = _cpu_time_acc / _cpu_frame_count;
    if (draw_time_plot)
    {
      memmove(&cpu_time_data[0], &cpu_time_data[1], (res - 1) * sizeof(float));
      cpu_time_data[res - 1] = static_cast<float>(_cpu_time_avg);
    }
    _cpu_time_acc = 0;
    _cpu_frame_count = 0;
  }

  _gpu_time_acc += _gpu_time;
  _gpu_frame_count++;
  if (_gpu_time_acc > gpu_interval)
  {
    _gpu_time_avg = _gpu_time_acc / _gpu_frame_count;
    if (draw_time_plot)
    {
      memmove(&gpu_time_data[0], &gpu_time_data[1], (res - 1) * sizeof(float));
      gpu_time_data[res - 1] = static_cast<float>(_gpu_time_avg * 1e-6);
    }
    _gpu_time_acc = 0;
    _gpu_frame_count = 0;
  }

  ImGui::InputInt("Rendered Objects:", &_rendered_objects, 0, 0,
                  ImGuiInputTextFlags_ReadOnly);
  ImGui::Checkbox("Draw Time plots", &draw_time_plot);

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
    ImGui::PlotHistogram("CPU Times", cpu_time_data,
                         IM_ARRAYSIZE(cpu_time_data), 0,
                         fmt::format("Avg: {:2f}", _cpu_time_avg).c_str(),
                         0, FLT_MAX, ImVec2(0, 80));
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
    ImGui::PlotHistogram(
        "GPU Times", gpu_time_data, IM_ARRAYSIZE(gpu_time_data), 0,
        fmt::format("Avg: {:2f}", static_cast<float>(_gpu_time_avg * 1e-6)).c_str(), 0,
        FLT_MAX, ImVec2(0, 80));
  }
}
}  // namespace nell
