#include <freeflight_controller.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mesh.hpp>
#include <meshlet_triangle_mesh.hpp>
#include <model_import_system.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/mesh_shader_scene.hpp>
#include <transform.hpp>
#include <triangle_mesh_draw_meshtasks_objects.hpp>
#include <ui_entity_draw_system.hpp>
#include <utils/gl_utils.hpp>
#include <utils/io_utils.hpp>

static constexpr unsigned int kPipelineCount = 1;
static constexpr GLuint kTestPipeline = 0;

static constexpr auto kAttrPosition = "position";
static constexpr auto kAttrNormal = "normal";
static constexpr unsigned int kShaderProgramCount = 1;

static constexpr auto kVerticesBindloc = 0;
static constexpr auto kNormalBindloc = 1;

namespace nell
{
MeshShaderScene::MeshShaderScene()
    : _basic_mesh_shader(
          0, GL_MESH_SHADER_NV,
          std::string(kShaderPath) + std::string("basic_shader.mesh"),
          std::vector<gl_utils::InterfaceToName>{}),
      _basic_fragment_shader(
          0, GL_FRAGMENT_SHADER,
          std::string(kShaderPath) + std::string("basic_shader.frag"),
          std::vector<gl_utils::InterfaceToName>{})
{
  _resource_location_maps.reserve(kShaderProgramCount);

  glCreateProgramPipelines(kPipelineCount, _program_pipeline);
  glUseProgramStages(_program_pipeline[kTestPipeline], GL_MESH_SHADER_BIT_NV,
                     static_cast<GLuint>(_basic_mesh_shader));
  glUseProgramStages(_program_pipeline[kTestPipeline], GL_FRAGMENT_SHADER_BIT,
                     static_cast<GLuint>(_basic_fragment_shader));
}

void MeshShaderScene::populate(Scene *scene, entt::registry &reg)
{
  auto entity = reg.create();
  reg.assign<comp::EntityName>(entity, "Armadillo");
  auto &asp = reg.assign<comp::ModelSource>(entity);
  auto &tf = reg.assign<comp::Transform>(entity);
  asp.path = "Armadillo.ply";

  systems::importModelsFromSource<comp::MeshletTriangleMesh>(reg);
}

/*
 * Entities hold Buffers.
 * Classes for Shaders, hold necessary data (like vao), consist of Systems to
 * initialize and render.
 *
 * TODO: Implement callbacks instead of calling render->render->render of all
 * those subsystems.
 */

void MeshShaderScene::setup(Scene *scene, entt::registry &reg)
{
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto mesh_view = reg.view<comp::MeshletTriangleMesh>();

  for (auto entity : mesh_view)
  {
    auto &meshlet_triangle_mesh =
        mesh_view.get<comp::MeshletTriangleMesh>(entity);
    auto &draw_elements_buffer =
        reg.assign<comp::TriangleMeshDrawMeshtasksObjects>(entity);

    spdlog::debug(
        "First triangle vertices:\n{:d}: {: f}, {: f}, {: f}\n{:d}: {: f}, {: f}, {: f}\n{:d}: {: f}, {: f}, {: f}",
        meshlet_triangle_mesh.indices[0],
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[0]].x,
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[0]].y,
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[0]].z,
        meshlet_triangle_mesh.indices[1],
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[1]].x,
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[1]].y,
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[1]].z,
        meshlet_triangle_mesh.indices[2],
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[2]].x,
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[2]].y,
        meshlet_triangle_mesh.vertices[meshlet_triangle_mesh.indices[2]].z);
    auto &vssbo = draw_elements_buffer.vertex_ssbo;
    auto &nssbo = draw_elements_buffer.normal_ssbo;
    auto &issbo = draw_elements_buffer.index_ssbo;

    // Setup VSSBO
    glCreateBuffers(1, &vssbo);
    glNamedBufferStorage(vssbo, meshlet_triangle_mesh.getVerticesSize(),
                         meshlet_triangle_mesh.vertices.data(),
                         GL_DYNAMIC_STORAGE_BIT);

    // Setup NSSBO
    glCreateBuffers(1, &nssbo);
    glNamedBufferStorage(nssbo, meshlet_triangle_mesh.getNormalsSize(),
                         meshlet_triangle_mesh.normals.data(),
                         GL_DYNAMIC_STORAGE_BIT);

    // Setup ISSBO
    glCreateBuffers(1, &issbo);
    glNamedBufferStorage(issbo, meshlet_triangle_mesh.getIndicesSize(),
                         meshlet_triangle_mesh.indices.data(),
                         GL_DYNAMIC_STORAGE_BIT);
  }
}
void MeshShaderScene::resize(int w, int h) {}

void MeshShaderScene::update(Scene *scene, entt ::registry &reg,
                             const input::NellInputList &input_list,
                             const double &time, const double &delta_time)
{
  scene->drawComponentImGui<comp::MeshletTriangleMesh,
                            comp::TriangleMeshDrawMeshtasksObjects>();
}
void MeshShaderScene::render(Scene *scene, entt::registry &reg,
                             entt::entity &camera_entity, const double &time,
                             const double &delta_time)
{
  auto perspective_camera = reg.get<comp::PerspectiveCamera>(camera_entity);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ### Render Meshes with Basic Shader ###
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Get Entities with Model of BasisShadingMesh and iterate
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto render_objects =
      reg.view<comp::MeshletTriangleMesh,
               comp::TriangleMeshDrawMeshtasksObjects, comp::Transform>();
  const auto vertex_size_t = comp::MeshletTriangleMesh::getVertexSizeT();
  const auto normal_size_t = comp::MeshletTriangleMesh::getNormalSizeT();

  glUseProgram(0);  // Unbind any bind programs to use program pipeline instead.

  for (auto entity : render_objects)
  {
    auto [triangle_mesh, gpu_buffers, transform] =
        render_objects
            .get<comp::MeshletTriangleMesh,
                 comp::TriangleMeshDrawMeshtasksObjects, comp::Transform>(
                entity);

    auto mvp_matrix = perspective_camera.getViewProjectionMatrix() *
                      transform.getTransformation();
    auto mvp_matrix_ptr = glm::value_ptr(mvp_matrix);

    glBindProgramPipeline(_program_pipeline[kTestPipeline]);

    auto &vssbo = gpu_buffers.vertex_ssbo;
    auto &nssbo = gpu_buffers.normal_ssbo;
    auto &issbo = gpu_buffers.index_ssbo;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, issbo);

    glProgramUniformMatrix4fv(static_cast<GLuint>(_basic_mesh_shader), 0, 1,
                              GL_FALSE, mvp_matrix_ptr);

    GLuint mesh_task_count = (triangle_mesh.indices.size() / 3) / 32;
    mesh_task_count =
        glm::min(gpu_buffers.limit_mesh_task_count, mesh_task_count);

    glDrawMeshTasksNV(0, mesh_task_count);
  }
}
}  // namespace nell
