#pragma once
#include <entt/entity/registry.hpp>
#include "glad.h"
#include "shader_source.hpp"
#include "asset_source_path.hpp"

namespace nell
{
inline void populateBasicScene(entt::registry &reg)
{
  auto entity = reg.create();
  auto &asp = reg.assign<AssetSourcePath>(entity);
  auto &ss = reg.assign<ShaderSource>(entity);
  asp.path = "Test";
  ss.shader_type = GL_VERTEX_SHADER;
  ss.shader_path = "vertexshader";

  auto entity_two = reg.create();
  auto &asp_two = reg.assign<AssetSourcePath>(entity_two);
  asp_two.path = "Test2";

  auto entity_three = reg.create();
  auto &ss_two = reg.assign<ShaderSource>(entity_three);

  ss_two.shader_type = GL_VERTEX_SHADER;
  ss_two.shader_path = "vertexshader-two";
}
}  // namespace nell
