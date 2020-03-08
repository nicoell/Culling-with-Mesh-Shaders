#pragma once
#include <glad/glad.h>

#include <entt/entity/registry.hpp>
#include <nell/input.hpp>
#include <nell/scene.hpp>
#include <nell/scenes/scene_impl.hpp>
#include <utils/gl_utils.hpp>

namespace nell
{
class BasicScene final : public SceneImpl
{
 public:
  explicit BasicScene();
  void populate(Scene*, entt::registry& reg) override;
  void setup(Scene*, entt::registry& reg) override;
  void resize(int w, int h) override;
  void update(Scene* scene, entt::registry& reg,
              const input::NellInputList& input_list, const double& time,
              const double& delta_time) override;
  void render(Scene* scene, entt::registry& reg, entt::entity& camera_entity,
              const double& time, const double& delta_time) override;

 private:
  GLuint _program_pipeline[1];
  GLuint _vertex_array_object;

  gl_utils::ShaderDefinition _basic_vertex_shader;
  gl_utils::ShaderDefinition _basic_fragment_shader;

  std::vector<std::map<const char*, GLuint>> _resource_location_maps;
};

}  // namespace nell
