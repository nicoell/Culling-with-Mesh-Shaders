#pragma once
#include <imgui/misc/imgui_stdlib.h>
#include <spdlog/spdlog.h>

#include <string>

namespace nell::comp
{
struct ModelSource
{
  std::string path;
};

template <typename Archive>
void serialize(Archive &archive, ModelSource &asset_source_path)
{
  archive(asset_source_path.path);
}

inline void drawComponentImpl(ModelSource &asset_source_path)
{
  if (ImGui::TreeNode("Model Component"))
  {
    ImGui::InputText("Source Path", &asset_source_path.path, ImGuiInputTextFlags_ReadOnly);
    ImGui::TreePop();
  }
}

}  // namespace nell::comp
