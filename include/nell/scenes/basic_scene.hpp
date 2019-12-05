#pragma once
#include <glad/glad.h>
#include <entt/entity/registry.hpp>
#include <nell/components/asset_source_path.hpp>
#include <nell/components/shaders.hpp>
#include <nell/scenes/scene_impl.hpp>
#include <input.hpp>

namespace nell
{
class BasicScene final : public SceneImpl
{
 public:
  BasicScene();
  void populate(entt::registry &reg) override;
  void setup(entt::registry &reg) override;
  void resize(int w, int h) override;
  void update(const double& time, const double& delta_time,
              const input::NellInputList& input_list, entt::registry& reg) override;
  void render(const double& time,
              const double& delta_time, entt::registry& reg) override;

  
 private:
  GLuint* _shader_programs;
  GLuint* _program_pipeline;
};

}  // namespace nell
