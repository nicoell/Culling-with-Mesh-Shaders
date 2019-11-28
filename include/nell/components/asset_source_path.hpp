#pragma once
#include <string>
#include <spdlog/spdlog.h>
#include <imgui/misc/imgui_stdlib.h>

namespace nell
{
struct AssetSourcePath
{
  std::string path;
};

template <typename Archive>
void serialize(Archive &archive, AssetSourcePath &asset_source_path)
{
  archive(asset_source_path.path);
}

inline void drawComponentUi(AssetSourcePath &asset_source_path)
{
  ImGui::InputText("Source Path", &asset_source_path.path);
  //ImGui::Text(asset_source_path.path.data());
  /*spdlog::debug("Draw Asset Source Path UI");
  spdlog::debug(asset_source_path.path);*/
}

}  // namespace nell
