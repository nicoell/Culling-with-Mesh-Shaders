#pragma once
#include <imgui/imgui.h>
#include <entt/entity/registry.hpp>
#include <nell/components/asset_source_path.hpp>

namespace nell
{
template <typename Component>
void updateComponentUi(entt::registry &reg, entt::entity &entity)
{
  if (auto *comp = reg.try_get<Component>(entity); comp)
  {
    drawComponentUi(*comp);
  }
}

template <typename... Component>
void updateEntityUi(entt::registry &reg)
{
  std::uint64_t dt;
  ImGui::Begin("Entity Browser");
  reg.each([&](auto entity) {
    if (ImGui::TreeNode((void *)(intptr_t)entt::to_integer(entity), "Entity %d",
                        entt::to_integer(entity)))
    {
      (updateComponentUi<Component>(reg, entity), ...);

      ImGui::TreePop();
    }
  });
  ImGui::End();
}

};  // namespace nell
