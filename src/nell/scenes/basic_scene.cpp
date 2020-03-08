#include <basic_shading_objects.hpp>
#include <freeflight_controller.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mesh.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/basic_scene.hpp>
#include <transform.hpp>
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
BasicScene::BasicScene()
    : _basic_vertex_shader(
          0, GL_VERTEX_SHADER,
          std::string(kShaderPath) + std::string("basic_shader.vert"),
          std::vector<gl_utils::InterfaceToName>{
              {{GL_PROGRAM_INPUT, kAttrPosition},
               {GL_PROGRAM_INPUT, kAttrNormal}}}),
      _basic_fragment_shader(
          0, GL_FRAGMENT_SHADER,
          std::string(kShaderPath) + std::string("basic_shader.frag"),
          std::vector<gl_utils::InterfaceToName>{})
{
  _resource_location_maps.reserve(kShaderProgramCount);

  glCreateProgramPipelines(kPipelineCount, _program_pipeline);
  glUseProgramStages(_program_pipeline[kTestPipeline], GL_VERTEX_SHADER_BIT,
                     static_cast<GLuint>(_basic_vertex_shader));
  glUseProgramStages(_program_pipeline[kTestPipeline], GL_FRAGMENT_SHADER_BIT,
                     static_cast<GLuint>(_basic_fragment_shader));
}

void BasicScene::populate(Scene *scene, entt::registry &reg)
{
  auto entity = reg.create();
  auto &asp = reg.assign<comp::ModelSource>(entity);
  auto &tf = reg.assign<comp::Transform>(entity);
  asp.path = "Armadillo.ply";
}

/*
 * Entities hold Buffers.
 * Classes for Shaders, hold necessary data (like vao), consist of Systems to
 * initialize and render.
 *
 * TODO: Implement callbacks instead of calling render->render->render of all
 * those subsystems.
 */

void BasicScene::setup(Scene *scene, entt::registry &reg)
{
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ### Setup VAO and Mesh Buffers ###
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Setup VAO
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  glCreateVertexArrays(1, &_vertex_array_object);
  // Enable vao attributes
  glEnableVertexArrayAttrib(_vertex_array_object,
                            _basic_vertex_shader[kAttrPosition]);
  glEnableVertexArrayAttrib(_vertex_array_object,
                            _basic_vertex_shader[kAttrNormal]);
  // Format vao attributes
  glVertexArrayAttribFormat(
      _vertex_array_object, _basic_vertex_shader[kAttrPosition],
      comp::BasicShadingMesh::getVertexSize(),
      comp::BasicShadingMesh::getVertexType(), GL_FALSE, 0);
  glVertexArrayAttribFormat(
      _vertex_array_object, _basic_vertex_shader[kAttrNormal],
      comp::BasicShadingMesh::getNormalSize(),
      comp::BasicShadingMesh::getNormalType(), GL_FALSE, 0);

  // Set binding location of attributes
  glVertexArrayAttribBinding(_vertex_array_object,
                             _basic_vertex_shader[kAttrPosition],
                             kVerticesBindloc);
  glVertexArrayAttribBinding(_vertex_array_object,
                             _basic_vertex_shader[kAttrNormal], kNormalBindloc);

  // Get Entities with Model of BasisShadingMesh and iterate, to set up Buffers
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto model_view = reg.view<comp::Model<comp::BasicShadingMesh>>();
  for (auto model_entity : model_view)
  {
    auto &model_comp = model_view.get(model_entity);

    for (auto &mesh : model_comp.mesh_list)
    {
      auto &vbo = mesh.bs_objects.vertex_buffer_object;
      auto &nbo = mesh.bs_objects.normal_buffer_object;
      auto &ibo = mesh.bs_objects.index_buffer_object;

      // Setup VBO
      glCreateBuffers(1, &vbo);
      glNamedBufferStorage(vbo, mesh.getVerticesSize(), mesh.vertices.data(),
                           0);
      //// Bind vbo to vao at bindloc
      // glVertexArrayVertexBuffer(_vertex_array_object, kVerticesBindloc, vbo,
      // 0,
      //                          comp::BasicShadingMesh::getVertexSize());

      // Setup NBO
      glCreateBuffers(1, &nbo);
      glNamedBufferStorage(nbo, mesh.getNormalsSize(), mesh.normals.data(), 0);
      //// Bind nbo to vao at bindloc
      // glVertexArrayVertexBuffer(_vertex_array_object, kNormalBindloc, nbo, 0,
      //                          comp::BasicShadingMesh::getNormalSize());

      // Setup IBO
      glCreateBuffers(1, &ibo);
      glNamedBufferStorage(ibo, mesh.getIndicesSize(), mesh.indices.data(), 0);
      //// Bind indexbuffer to vao
      // glVertexArrayElementBuffer(_vertex_array_object, ibo);
    }
  }
}
void BasicScene::resize(int w, int h) {}

void BasicScene::update(Scene *scene, entt ::registry &reg,
                        const input::NellInputList &input_list,
                        const double &time, const double &delta_time)
{
  scene->drawComponentImGui<comp::Model<comp::BasicShadingMesh>>();
}
void BasicScene::render(Scene *scene, entt::registry &reg,
                        entt::entity &camera_entity, const double &time,
                        const double &delta_time)
{
  auto perspective_camera = reg.get<comp::PerspectiveCamera>(camera_entity);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ### Render Meshes with Basic Shader ###
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Get Entities with Model of BasisShadingMesh and iterate
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  auto model_view =
      reg.view<comp::Model<comp::BasicShadingMesh>, comp::Transform>();
  const auto vertex_size_t = comp::BasicShadingMesh::getVertexSizeT();
  const auto normal_size_t = comp::BasicShadingMesh::getNormalSizeT();

  glUseProgram(0);  // Unbind any bind programs to use program pipeline instead.

  for (auto model_entity : model_view)
  {
    auto [model_comp, transform] =
        model_view.get<comp::Model<comp::BasicShadingMesh>, comp::Transform>(
            model_entity);

    auto mvp_matrix = perspective_camera.getViewProjectionMatrix() * transform.getTransformation();
    auto mvp_matrix_ptr = glm::value_ptr(mvp_matrix);

    for (const auto &mesh : model_comp.mesh_list)
    {
      
      glBindProgramPipeline(_program_pipeline[kTestPipeline]);

      auto &vbo = mesh.bs_objects.vertex_buffer_object;
      auto &nbo = mesh.bs_objects.normal_buffer_object;
      auto &ibo = mesh.bs_objects.index_buffer_object;

      // Bind vbo to vao at bindloc
      glVertexArrayVertexBuffer(_vertex_array_object, kVerticesBindloc, vbo, 0,
                                vertex_size_t);
      // Bind nbo to vao at bindloc
      glVertexArrayVertexBuffer(_vertex_array_object, kNormalBindloc, nbo, 0,
                                normal_size_t);
      // Bind indexbuffer to vao
      glVertexArrayElementBuffer(_vertex_array_object, ibo);

      glProgramUniformMatrix4fv(static_cast<GLuint>(_basic_vertex_shader), 0, 1, GL_FALSE, mvp_matrix_ptr);

      glBindVertexArray(_vertex_array_object);
      // Draw Call
      glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
    }
  }
}
}  // namespace nell
