#include <mesh.hpp>
#include <nell/definitions.hpp>
#include <nell/scenes/basic_scene.hpp>
#include <utils/gl_utils.hpp>
#include <utils/io_utils.hpp>

static constexpr unsigned int kPipelineCount = 1;
static constexpr GLuint kTestPipeline = 0;

static constexpr unsigned int kShaderProgramCount = 1;
static constexpr GLuint kBasicVertexShader = 0;
static constexpr auto kBasicVertexShaderPath = "basicVertexShader.vert";

namespace nell
{
BasicScene::BasicScene()
    : _shader_programs(new GLuint[kShaderProgramCount]),
      _program_pipeline(new GLuint[kPipelineCount])
{
  _shader_programs[kBasicVertexShader] = gl_utils::createShaderProgram(
      GL_VERTEX_SHADER,
      std::string(kShaderPath) + std::string(kBasicVertexShaderPath));

  glCreateProgramPipelines(kPipelineCount, _program_pipeline);

  glUseProgramStages(_program_pipeline[kTestPipeline], GL_VERTEX_SHADER_BIT,
                     _shader_programs[kBasicVertexShader]);
}

void BasicScene::populate(entt::registry &reg)
{
  auto entity = reg.create();
  auto &asp = reg.assign<AssetSourcePath>(entity);
  asp.path = "Armadillo.ply";

  auto entity_two = reg.create();
  auto &asp_two = reg.assign<AssetSourcePath>(entity_two);
  asp_two.path = "Test2";

}

void BasicScene::setup(entt::registry &reg)
{
  auto model_view = reg.view<comp::Model>();

  for (auto model_entity : model_view)
  {
    auto &model_comp = model_view.get(model_entity);
    for(auto mesh : model_comp.mesh_list)
    {
      
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
