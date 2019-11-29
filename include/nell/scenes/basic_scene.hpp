#pragma once
#include <glad/glad.h>
#include <entt/entity/registry.hpp>
#include <nell/components/asset_source_path.hpp>
#include <nell/components/shaders.hpp>

namespace nell
{
inline void populateBasicScene(entt::registry &reg)
{
  auto entity = reg.create();
  auto &asp = reg.assign<AssetSourcePath>(entity);
  auto &ss = reg.assign<Shaders>(entity);
  asp.path = "Armadillo.ply";
  ss.shader_groups.push_back(
      ShaderGroup{{GL_VERTEX_SHADER, "vertexshader"},
                  {GL_FRAGMENT_SHADER, "fragmentshader"}});
  ss.shader_groups.push_back(ShaderGroup{{GL_COMPUTE_SHADER, "computeshader"}});

  auto entity_two = reg.create();
  auto &asp_two = reg.assign<AssetSourcePath>(entity_two);
  asp_two.path = "Test2";

  auto entity_three = reg.create();
  auto &ss_two = reg.assign<Shaders>(entity_three);

  ss_two.shader_groups.push_back(ShaderGroup{
      {GL_MESH_SHADER_NV, "meshshader"}, {GL_TASK_SHADER_NV, "taskshader"}});
}
}  // namespace nell
