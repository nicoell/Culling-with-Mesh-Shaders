#include <freeflight_camera.hpp>
#include <mesh.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/basic_scene.hpp>
#include <opengl_mesh.hpp>
#include <transform.hpp>
#include <utils/gl_utils.hpp>
#include <utils/io_utils.hpp>

static constexpr unsigned int kPipelineCount = 1;
static constexpr GLuint kTestPipeline = 0;

static constexpr auto kAttrPosition = "position";
static constexpr auto kAttrNormal = "normal";
static constexpr unsigned int kShaderProgramCount = 1;

namespace nell
{
BasicScene::BasicScene()
    : _program_pipeline(new GLuint[kPipelineCount]),
      _basic_vertex_shader(
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
  auto model_view = reg.view<comp::Model>();

  for (auto model_entity : model_view)
  {
    auto &model_comp = model_view.get(model_entity);
    for (auto mesh : model_comp.mesh_list)
    {
      auto &vad_comp = reg.assign<comp::BasicVertexArrayData>(model_entity);
      auto &vao = vad_comp.vertex_array_object;
      auto &vbo = vad_comp.vertex_buffer_object;
      auto &nbo = vad_comp.normal_buffer_object;
      auto &ibo = vad_comp.index_buffer_object;

      const auto vertices_bindloc = 0;
      const auto normal_bindloc = 1;

      glCreateVertexArrays(1, &vao);
      // Enable vao attributes
      glEnableVertexArrayAttrib(vao, _basic_vertex_shader[kAttrPosition]);
      glEnableVertexArrayAttrib(vao, _basic_vertex_shader[kAttrNormal]);
      // Format vao attributes
      glVertexArrayAttribFormat(vao, _basic_vertex_shader[kAttrPosition],
                                mesh.getVertexSize(), mesh.getVertexType(),
                                GL_FALSE, 0);
      glVertexArrayAttribFormat(vao, _basic_vertex_shader[kAttrNormal],
                                mesh.getNormalSize(), mesh.getNormalType(),
                                GL_FALSE, 0);

      // Set binding location of attributes
      glVertexArrayAttribBinding(vao, _basic_vertex_shader[kAttrPosition],
                                 vertices_bindloc);
      glVertexArrayAttribBinding(vao, _basic_vertex_shader[kAttrNormal],
                                 normal_bindloc);

      // Setup VBO
      glCreateBuffers(1, &vbo);
      glNamedBufferStorage(vbo, mesh.getVerticesSize(), mesh.vertices.data(),
                           GL_STATIC_DRAW);
      // Setup NBO
      glCreateBuffers(1, &nbo);
      glNamedBufferStorage(nbo, mesh.getNormalsSize(), mesh.normals.data(),
                           GL_STATIC_DRAW);

      // Bind vbo to vao at bindloc
      glVertexArrayVertexBuffer(vao, vertices_bindloc, vbo, 0,
                                mesh.getVertexSize());

      // Bind nbo to vao at bindloc
      glVertexArrayVertexBuffer(vao, normal_bindloc, nbo, 0,
                                mesh.getNormalSize());

      // Setup IBO
      glCreateBuffers(1, &ibo);
      glNamedBufferStorage(ibo, mesh.getIndicesSize(), mesh.indices.data(),
                           GL_STATIC_DRAW);
      // Bind indexbuffer to vao
      glVertexArrayElementBuffer(vao, ibo);
    }
  }
}
 void BasicScene::resize(int w, int h) {}
 void BasicScene::update(const double &time, const double &delta_time,
                        const input::NellInputList &input_list,
                        entt ::registry &reg)
{
}
 void BasicScene::render(const double &time, const double &delta_time,
                        entt::registry &reg)
{
}
}  // namespace nell
