#include <glm/gtc/type_ptr.hpp>
#include <meshlet_triangle_mesh_objects.hpp>
#include <nell/components/meshlet_triangle_mesh.hpp>
#include <nell/components/transform.hpp>
#include <nell/components/triangle_mesh_draw_meshtasks_objects.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/culling_mesh_scene.hpp>
#include <nell/systems/model_import_system.hpp>
#include <utils/gl_utils.hpp>

static constexpr unsigned int kPipelineCount = 1;
static constexpr GLuint kTestPipeline = 0;

static constexpr unsigned int kShaderProgramCount = 1;

static constexpr auto kUniformViewOrigin = "view_origin";
// static constexpr auto kUniformViewUp = "view_up";
// static constexpr auto kUniformViewRight = "view_right";
static constexpr auto kUniformDebugValue = "debug_value";
static constexpr auto kUniformFrustumPlanes = "frustum_planes[0]";

static constexpr auto kUniformMvp = "mvp";
static constexpr auto kUniformModelMatrixTransposeInverse =
    "model_matrix_transpose_inverse";

namespace nell
{
CullingMeshScene::CullingMeshScene()
    : _culling_mesh_shader(
          0, GL_MESH_SHADER_NV,
          std::string(kShaderPath) + std::string("culling_shader.mesh")),
      _culling_task_shader(
          0, GL_TASK_SHADER_NV,
          std::string(kShaderPath) + std::string("culling_shader.task")),
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

  glUseProgram(0);  // Unbind any bind programs to use program pipeline instead.

  for (auto entity : render_objects)
  {
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

    auto model_view_transpose_inverse =
        model_matrix_transpose_inverse * view_matrix_transpose_inverse;

    auto frustum_planes_obj_space = frustum_planes;
    for (auto &frustum_plane : frustum_planes_obj_space)
    {
      auto plane_normal = glm::vec3(model_view_transpose_inverse * frustum_plane);
      auto inv_length = 1.0f / glm::length(plane_normal);
      frustum_plane = glm::vec4(plane_normal, frustum_plane.w);
      frustum_plane *= inv_length;
    }

    auto view_up_obj_space = glm::normalize(
        model_matrix_inverse * glm::vec4(cam_transform.getUp(), 0));
    auto view_right_obj_space = glm::normalize(
        model_matrix_inverse * glm::vec4(cam_transform.getRight(), 0));
    auto view_origin_obj_space =
        model_matrix_inverse * glm::vec4(cam_transform.getTranslation(), 1);

    glProgramUniformMatrix4fv(static_cast<GLuint>(_culling_mesh_shader),
                              _uniform_mvp_loc, 1, GL_FALSE,
                              glm::value_ptr(mvp_matrix));

    glProgramUniformMatrix4fv(static_cast<GLuint>(_culling_mesh_shader),
                              _uniform_model_matrix_transpose_inverse, 1,
                              GL_FALSE,
                              glm::value_ptr(model_matrix_transpose_inverse));

    glProgramUniform1f(static_cast<GLuint>(_culling_task_shader),
                       _uniform_debug_value_loc,
                       0.f);//meshlet_triangle_mesh.debug_value);

    // glProgramUniform3fv(static_cast<GLuint>(_culling_task_shader),
    //                    _uniform_view_up_loc, 1,
    //                    glm::value_ptr(view_up_obj_space));

    // glProgramUniform3fv(static_cast<GLuint>(_culling_task_shader),
    //                    _uniform_view_right_loc, 1,
    //                    glm::value_ptr(view_right_obj_space));

    glProgramUniform3fv(static_cast<GLuint>(_culling_task_shader),
                        _uniform_view_origin_loc, 1,
                        glm::value_ptr(view_origin_obj_space));

    glProgramUniform4fv(static_cast<GLuint>(_culling_task_shader),
                        _uniform_frustum_planes_loc, 6,
                        glm::value_ptr(frustum_planes_obj_space[0]));

    GLuint task_count =
        (meshlet_triangle_mesh.meshlet_descriptors.size() / 32) +
        (meshlet_triangle_mesh.meshlet_descriptors.size() % 32 != 0);

    glDrawMeshTasksNV(0, task_count);
  }
}
}  // namespace nell
