#include <glm/gtc/type_ptr.hpp>
#include <nell/components/transform.hpp>
#include <nell/components/triangle_mesh.hpp>
#include <nell/components/triangle_mesh_draw_meshtasks_objects.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/mesh_shader_scene.hpp>
#include <nell/systems/model_import_system.hpp>
#include <utils/gl_utils.hpp>

static constexpr unsigned int kPipelineCount = 1;
static constexpr GLuint kRenderPipeline = 0;

static constexpr auto kVertexSsbo = "vertex_ssbo";
static constexpr auto kNormalSsbo = "normal_ssbo";
static constexpr auto kIndexSsbo = "index_ssbo";

static constexpr auto kUniformMvp = "mvp";

namespace nell
{
MeshShaderScene::MeshShaderScene()
    : _basic_mesh_shader(
          0, GL_MESH_SHADER_NV,
          std::string(kShaderPath) + std::string("basic_shader.mesh")),
      _basic_fragment_shader(
          0, GL_FRAGMENT_SHADER,
          std::string(kShaderPath) + std::string("basic_shader.frag"))
{
  glCreateProgramPipelines(kPipelineCount, _program_pipeline);
  glUseProgramStages(_program_pipeline[kRenderPipeline], GL_MESH_SHADER_BIT_NV,
                     static_cast<GLuint>(_basic_mesh_shader));
  glUseProgramStages(_program_pipeline[kRenderPipeline], GL_FRAGMENT_SHADER_BIT,
                     static_cast<GLuint>(_basic_fragment_shader));

  _vertex_ssbo_binding_index =
      _basic_mesh_shader.getShaderStorageBlock(kVertexSsbo).buffer_binding;
  _normal_ssbo_binding_index =
      _basic_mesh_shader.getShaderStorageBlock(kNormalSsbo).buffer_binding;
  _index_ssbo_binding_index =
      _basic_mesh_shader.getShaderStorageBlock(kIndexSsbo).buffer_binding;

  _uniform_mvp_loc = _basic_mesh_shader.getUniform(kUniformMvp).location;
}

void MeshShaderScene::populate(Scene *scene, entt::registry &reg)
{
  auto entity = reg.create();
  reg.assign<comp::EntityName>(entity, "Armadillo");
  auto &asp = reg.assign<comp::ModelSource>(entity);
  auto &tf = reg.assign<comp::Transform>(entity);
  asp.path = "Armadillo.ply";

  systems::importModelsFromSource<comp::TriangleMesh>(reg);
}

void MeshShaderScene::setup(Scene *scene, entt::registry &reg)
{
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto mesh_view = reg.view<comp::TriangleMesh>();

  for (auto entity : mesh_view)
  {
    auto &meshlet_triangle_mesh = mesh_view.get<comp::TriangleMesh>(entity);
    auto &draw_elements_buffer =
        reg.assign<comp::TriangleMeshDrawMeshtasksObjects>(entity);

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
  scene->drawComponentImGui<comp::TriangleMesh,
                            comp::TriangleMeshDrawMeshtasksObjects>();
}
void MeshShaderScene::render(Scene *scene, entt::registry &reg,
                             entt::entity &camera_entity, const double &time,
                             const double &delta_time)
{
  auto &perspective_camera = reg.get<comp::PerspectiveCamera>(camera_entity);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ### Render Meshes with Basic Shader ###
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Get Entities with Model of BasisShadingMesh and iterate
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto render_objects =
      reg.view<comp::TriangleMesh, comp::TriangleMeshDrawMeshtasksObjects,
               comp::Transform>();
  const auto vertex_size_t = comp::TriangleMesh::getVertexSizeT();
  const auto normal_size_t = comp::TriangleMesh::getNormalSizeT();

  glUseProgram(0);  // Unbind any bind programs to use program pipeline instead.

  for (auto entity : render_objects)
  {
    auto [triangle_mesh, gpu_buffers, transform] =
        render_objects
            .get<comp::TriangleMesh, comp::TriangleMeshDrawMeshtasksObjects,
                 comp::Transform>(entity);

    auto mvp_matrix = perspective_camera.getViewProjectionMatrix() *
                      transform.getTransformation();
    auto mvp_matrix_ptr = glm::value_ptr(mvp_matrix);

    glBindProgramPipeline(_program_pipeline[kRenderPipeline]);

    auto &vssbo = gpu_buffers.vertex_ssbo;
    auto &nssbo = gpu_buffers.normal_ssbo;
    auto &issbo = gpu_buffers.index_ssbo;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _vertex_ssbo_binding_index,
                     vssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _normal_ssbo_binding_index,
                     nssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _index_ssbo_binding_index,
                     issbo);

    glProgramUniformMatrix4fv(static_cast<GLuint>(_basic_mesh_shader),
                              _uniform_mvp_loc, 1, GL_FALSE, mvp_matrix_ptr);

    GLuint mesh_task_count = (triangle_mesh.indices.size() / 3) / 32;
    mesh_task_count =
        glm::min(gpu_buffers.limit_mesh_task_count, mesh_task_count);

    glDrawMeshTasksNV(0, mesh_task_count);
  }
}

void MeshShaderScene::drawSceneImGui (Scene *scene, entt::registry &reg) {}

void MeshShaderScene::drawStatsImGui (Scene *scene, entt::registry &reg,
  const double &time, const double &delta_time)
{
  
}
}  // namespace nell
