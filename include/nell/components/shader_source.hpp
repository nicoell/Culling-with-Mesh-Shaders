#pragma once
#include <glad/glad.h>
#include <string>

namespace nell
{
struct ShaderSource
{
  GLenum shader_type;
  std::string shader_path;
};

template <typename Archive>
void serialize(Archive &archive, ShaderSource &shader_source)
{
  archive(shader_source.shader_type, shader_source.shader_path);
}

inline void drawComponentUi(ShaderSource &shader_source)
{
  ImGui::Text(shader_source.shader_path.data());
}
}  // namespace nell