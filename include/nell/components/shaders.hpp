#pragma once
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/misc/imgui_stdlib.h>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <map>
#include <string>
#include <vector>

namespace nell
{
typedef std::map<GLenum, std::string> ShaderGroup;

struct Shaders
{
  std::vector<ShaderGroup> shader_groups;
};

template <typename Archive>
void serialize(Archive &archive, Shaders &shaders)
{
  archive(shaders.shader_groups);
}

inline void drawComponentImpl(Shaders &shaders)
{
  if (ImGui::TreeNode("Shader Groups"))
  {
    for (int i = 0; i < shaders.shader_groups.size(); ++i)
    {
      if (ImGui::TreeNode((void *)(intptr_t)i, "Shader %d", i))
      {
        for (auto &shader_group_pair : shaders.shader_groups[i])
        {
          ImGui::PushID(&shader_group_pair.first);
          ImGui::Text("%s: %d",
                      "Type", shader_group_pair.first);
          ImGui::InputText("Path", &shader_group_pair.second);
          ImGui::PopID();
        }
        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }
}
}  // namespace nell