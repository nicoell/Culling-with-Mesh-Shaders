#pragma once
#include <imgui/misc/imgui_stdlib.h>
#include <spdlog/spdlog.h>
#include <ui_drawable.hpp>
#include <string>

namespace nell::comp
{
struct ModelSource : UiDrawable
{
  void drawImGui () override;
  std::string path;
};

inline void ModelSource::drawImGui ()
{
  if (ImGui::TreeNode("Model Component"))
  {
    ImGui::InputText("Source Path", &path,
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::TreePop();
  }
}

template <typename Archive>
void serialize(Archive &archive, ModelSource &asset_source_path)
{
  archive(asset_source_path.path);
}

}  // namespace nell::comp
