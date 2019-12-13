#pragma once
#include <glad/glad.h>

#include <entt/entity/registry.hpp>
#include <input.hpp>
#include <nell/components/model_source.hpp>
#include <nell/components/shaders.hpp>
#include <nell/scenes/scene_impl.hpp>
#include <utils/gl_utils.hpp>

namespace nell
{
class BasicScene final : public SceneImpl
{
 public:
  BasicScene();
  void populate(entt::registry& reg) override;
  void setup(entt::registry& reg) override;
  void resize(int w, int h) override;
  void update(const double& time, const double& delta_time,
              const input::NellInputList& input_list,
              entt::registry& reg) override;
  void render(const double& time, const double& delta_time,
              entt::registry& reg) override;

 private:
  GLuint* _program_pipeline;

  gl_utils::ShaderDefinition _basic_vertex_shader;
  gl_utils::ShaderDefinition _basic_fragment_shader;

  std::vector<std::map<const char*, GLuint>> _resource_location_maps;
};

}  // namespace nell
