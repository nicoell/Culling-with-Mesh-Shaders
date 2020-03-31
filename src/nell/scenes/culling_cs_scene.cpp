#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <nell/components/meshlet_triangle_mesh.hpp>
#include <nell/components/meshlet_triangle_mesh_cs_objects.hpp>
#include <nell/components/transform.hpp>
#include <nell/components/triangle_mesh_draw_meshtasks_objects.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/culling_cs_scene.hpp>
#include <nell/systems/model_import_system.hpp>
#include <utils/gl_utils.hpp>
#include <utils/glm_utils.hpp>

static constexpr unsigned int kPipelineCount = 2;
static constexpr GLuint kComputeCullingPipeline = 0;
static constexpr GLuint kRenderPipeline = 1;

static constexpr auto kAttrPosition = "position";
static constexpr auto kAttrNormal = "normal";
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

static constexpr auto kVerticesBindingIndex = 0;
static constexpr auto kNormalBindingIndex = 1;

namespace nell
{
CullingCsScene::CullingCsScene()
    : _swap_id(),
      _disable_frustum_culling(false),
      _freeze_frustum_culling(false),
      _disable_backface_culling(false),
      _freeze_backface_culling(false),
      _culling_compute_shader(
          0, GL_COMPUTE_SHADER,
          std::string(kShaderPath) + std::string("cs_culling.comp")),
      _basic_vert_shader(
          0, GL_VERTEX_SHADER,
          std::string(kShaderPath) + std::string("basic_shader.vert")),
      _basic_frag_shader(
          0, GL_FRAGMENT_SHADER,
          std::string(kShaderPath) + std::string("basic_shader.frag"))
{
  glCreateProgramPipelines(kPipelineCount, _program_pipeline);
  glUseProgramStages(_program_pipeline[kComputeCullingPipeline],
                     GL_COMPUTE_SHADER_BIT,
                     static_cast<GLuint>(_culling_compute_shader));
  glUseProgramStages(_program_pipeline[kRenderPipeline], GL_VERTEX_SHADER_BIT,
                     static_cast<GLuint>(_basic_vert_shader));
  glUseProgramStages(_program_pipeline[kRenderPipeline], GL_FRAGMENT_SHADER_BIT,
                     static_cast<GLuint>(_basic_frag_shader));

  _uniform_view_origin_loc =
      _culling_compute_shader.getUniform(kUniformViewOrigin).location;
  //_uniform_view_up_loc =
  //  _culling_compute_shader.getUniform(kUniformViewUp).location;
  //_uniform_view_right_loc =
  //  _culling_compute_shader.getUniform(kUniformViewRight).location;
  _uniform_frustum_planes_loc =
      _culling_compute_shader.getUniform(kUniformFrustumPlanes).location;
  _uniform_debug_value_loc =
      _culling_compute_shader.getUniform(kUniformDebugValue).location;
  _uniform_disable_frustum_culling_loc =
      _culling_compute_shader.getUniform(kUniformDisableFrustumCulling)
          .location;
  _uniform_disable_backface_culling_loc =
      _culling_compute_shader.getUniform(kUniformDisableBackfaceCulling)
          .location;

  _position_attr_loc =
      _basic_vert_shader.getProgramInput(kAttrPosition).location;
  _normal_attr_loc = _basic_vert_shader.getProgramInput(kAttrNormal).location;

  _uniform_mvp_loc = _basic_vert_shader.getUniform(kUniformMvp).location;
  _uniform_model_matrix_transpose_inverse =
      _basic_vert_shader.getUniform(kUniformModelMatrixTransposeInverse)
          .location;
}

void CullingCsScene::populate(Scene *scene, entt::registry &reg)
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

  systems::importModelsFromSource<comp::MeshletTriangleMesh>(reg);
}

void CullingCsScene::setup(Scene *scene, entt::registry &reg)
{
  // Setup VAO
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  glCreateVertexArrays(1, &_vertex_array_object);
  // Enable vao attributes
  glEnableVertexArrayAttrib(_vertex_array_object, _position_attr_loc);
  glEnableVertexArrayAttrib(_vertex_array_object, _normal_attr_loc);
  // Format vao attributes
  glVertexArrayAttribFormat(_vertex_array_object, _position_attr_loc,
                            comp::MeshletTriangleMesh::getVertexSize(),
                            comp::MeshletTriangleMesh::getVertexType(),
                            GL_FALSE, 0);
  glVertexArrayAttribFormat(_vertex_array_object, _normal_attr_loc,
                            comp::MeshletTriangleMesh::getNormalSize(),
                            comp::MeshletTriangleMesh::getNormalType(),
                            GL_FALSE, 0);

  // Set binding location of attributes
  glVertexArrayAttribBinding(_vertex_array_object, _position_attr_loc,
                             kVerticesBindingIndex);
  glVertexArrayAttribBinding(_vertex_array_object, _normal_attr_loc,
                             kNormalBindingIndex);
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto mesh_view = reg.view<comp::MeshletTriangleMesh>();

  comp::DrawElementsIndirectCommand indirect_commands[1];
  indirect_commands[0] = {.count = 0,
                          .instanceCount = 1,
                          .firstIndex = 0,
                          .baseVertex = 0,
                          .baseInstance = 0};

  for (auto entity : mesh_view)
  {
    auto &meshlet_triangle_mesh =
        mesh_view.get<comp::MeshletTriangleMesh>(entity);
    auto &meshlet_triangle_mesh_cs_buffers =
        reg.assign<comp::MeshletTriangleMeshCsObjects>(entity);

    auto &mdesc_ssbo = meshlet_triangle_mesh_cs_buffers.meshlet_descriptor_ssbo;
    auto &vbo = meshlet_triangle_mesh_cs_buffers.vertex_bo;
    auto &nbo = meshlet_triangle_mesh_cs_buffers.normal_bo;
    auto &ibo = meshlet_triangle_mesh_cs_buffers.index_ssbo;
    auto &visible_i_ssbo = meshlet_triangle_mesh_cs_buffers.visible_index_ssbo;
    auto &draw_indirect_ssbo =
        meshlet_triangle_mesh_cs_buffers.draw_indirect_ssbo;

    // Setup MDESC SSBO
    glCreateBuffers(1, &mdesc_ssbo);
    glNamedBufferStorage(mdesc_ssbo,
                         meshlet_triangle_mesh.getMeshletDescriptorSize(),
                         meshlet_triangle_mesh.meshlet_descriptors.data(), 0);

    // Setup VBO
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, meshlet_triangle_mesh.getVerticesSize(),
                         meshlet_triangle_mesh.vertices.data(), 0);

    // Setup NBO
    glCreateBuffers(1, &nbo);
    glNamedBufferStorage(nbo, meshlet_triangle_mesh.getNormalsSize(),
                         meshlet_triangle_mesh.normals.data(), 0);

    // Setup IBO
    glCreateBuffers(1, &ibo);
    glNamedBufferStorage(ibo, meshlet_triangle_mesh.getIndicesSize(),
                         meshlet_triangle_mesh.indices.data(), 0);

    // Setup VISIBLE_ISSBO
    glCreateBuffers(1, &visible_i_ssbo);
    glNamedBufferStorage(visible_i_ssbo, meshlet_triangle_mesh.getIndicesSize(),
                         meshlet_triangle_mesh.indices.data(),
                         GL_DYNAMIC_STORAGE_BIT);

    // Setup DRAW_INDIRECT_SSBO
    glCreateBuffers(1, &draw_indirect_ssbo);
    glNamedBufferStorage(draw_indirect_ssbo,
                         1 * sizeof(comp::DrawElementsIndirectCommand),
                         indirect_commands, GL_DYNAMIC_STORAGE_BIT);
  }

  // Timer Queries
  glGenQueries(2, _gpu_timer_query);
  // Dummy query
  glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.pong()]);
  glEndQuery(GL_TIME_ELAPSED);
}
void CullingCsScene::resize(int w, int h) {}

void CullingCsScene::update(Scene *scene, entt ::registry &reg,
                            const input::NellInputList &input_list,
                            const double &time, const double &delta_time)
{
  scene->drawComponentImGui<comp::MeshletTriangleMesh,
                            comp::TriangleMeshDrawMeshtasksObjects>();
}
void CullingCsScene::render(Scene *scene, entt::registry &reg,
                            entt::entity &camera_entity, const double &time,
                            const double &delta_time)
{
  _cpu_time = glfwGetTime();
  glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.ping()]);

  auto [perspective_camera, cam_transform] =
      reg.get<comp::PerspectiveCamera, comp::Transform>(camera_entity);
  const auto view_proj_matrix = perspective_camera.getViewProjectionMatrix();
  auto frustum_planes = perspective_camera.getFrustumPlanes();
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ### Render Meshes with Basic Shader ###
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Get Entities with Model of BasisShadingMesh and iterate
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto render_objects =
      reg.view<comp::MeshletTriangleMesh, comp::MeshletTriangleMeshCsObjects,
               comp::Transform>();
  const auto vertex_size_t = comp::MeshletTriangleMesh::getVertexSizeT();
  const auto normal_size_t = comp::MeshletTriangleMesh::getNormalSizeT();

  _rendered_objects = 0;

  glUseProgram(0);  // Unbind any bind programs to use program pipeline instead.

  for (auto entity : render_objects)
  {
    _rendered_objects++;

    auto [meshlet_triangle_mesh, meshlet_triangle_mesh_cs_buffers, transform] =
        render_objects.get<comp::MeshletTriangleMesh,
                           comp::MeshletTriangleMeshCsObjects, comp::Transform>(
            entity);

    auto &mdesc_ssbo = meshlet_triangle_mesh_cs_buffers.meshlet_descriptor_ssbo;
    auto &vbo = meshlet_triangle_mesh_cs_buffers.vertex_bo;
    auto &nbo = meshlet_triangle_mesh_cs_buffers.normal_bo;
    auto &ibo = meshlet_triangle_mesh_cs_buffers.index_ssbo;
    auto &visible_i_ssbo = meshlet_triangle_mesh_cs_buffers.visible_index_ssbo;
    auto &draw_indirect_ssbo =
        meshlet_triangle_mesh_cs_buffers.draw_indirect_ssbo;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Compute Shader Pipeline
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    glBindProgramPipeline(_program_pipeline[kComputeCullingPipeline]);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ibo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mdesc_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, visible_i_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, draw_indirect_ssbo);

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

    glProgramUniform1f(static_cast<GLuint>(_culling_compute_shader),
                       _uniform_debug_value_loc, 0.f);

    glProgramUniform1i(static_cast<GLuint>(_culling_compute_shader),
                       _uniform_disable_frustum_culling_loc,
                       _disable_frustum_culling);

    glProgramUniform1i(static_cast<GLuint>(_culling_compute_shader),
                       _uniform_disable_backface_culling_loc,
                       _disable_backface_culling);

    // glProgramUniform3fv(static_cast<GLuint>(_culling_compute_shader),
    //                    _uniform_view_up_loc, 1,
    //                    glm::value_ptr(_cached_view_up));

    // glProgramUniform3fv(static_cast<GLuint>(_culling_compute_shader),
    //                    _uniform_view_right_loc, 1,
    //                    glm::value_ptr(_cached_view_right));

    glProgramUniform3fv(static_cast<GLuint>(_culling_compute_shader),
                        _uniform_view_origin_loc, 1,
                        glm::value_ptr(_cached_view_origin));

    glProgramUniform4fv(static_cast<GLuint>(_culling_compute_shader),
                        _uniform_frustum_planes_loc, 6,
                        glm::value_ptr(_cached_frustum_planes[0]));

    const GLuint dispatch_count =
        meshlet_triangle_mesh.meshlet_descriptors.size();

    glDispatchCompute(dispatch_count, 1, 1);
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Render Shader Pipeline
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    glBindProgramPipeline(_program_pipeline[kRenderPipeline]);

    // Bind vbo to vao at bindloc
    glVertexArrayVertexBuffer(_vertex_array_object, kVerticesBindingIndex, vbo,
                              0, vertex_size_t);
    // Bind nbo to vao at bindloc
    glVertexArrayVertexBuffer(_vertex_array_object, kNormalBindingIndex, nbo, 0,
                              normal_size_t);
    // Bind indexbuffer to vao
    glVertexArrayElementBuffer(_vertex_array_object, visible_i_ssbo);

    glProgramUniformMatrix4fv(static_cast<GLuint>(_basic_vert_shader),
                              _uniform_mvp_loc, 1, GL_FALSE,
                              glm::value_ptr(mvp_matrix));

    glProgramUniformMatrix4fv(static_cast<GLuint>(_basic_vert_shader),
                              _uniform_model_matrix_transpose_inverse, 1,
                              GL_FALSE,
                              glm::value_ptr(model_matrix_transpose_inverse));

    glBindVertexArray(_vertex_array_object);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, draw_indirect_ssbo);

    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);

    // Reset indirect command count
    constexpr GLuint reset_count = 0u;
    glClearNamedBufferSubData(draw_indirect_ssbo, GL_R32UI, 0, sizeof(GLuint),
                              GL_RED_INTEGER, GL_UNSIGNED_INT, &reset_count);
  }

  glEndQuery(GL_TIME_ELAPSED);
  glGetQueryObjectui64v(_gpu_timer_query[_swap_id.pong()], GL_QUERY_RESULT,
                        &_gpu_time);

  _cpu_time = (glfwGetTime() - _cpu_time) * 1000.0;
  _swap_id.swap();
}

void CullingCsScene::drawSceneImGui(Scene *scene, entt::registry &reg)
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

void CullingCsScene::drawStatsImGui(Scene *scene, entt::registry &reg,
                                    const double &time,
                                    const double &delta_time)
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
