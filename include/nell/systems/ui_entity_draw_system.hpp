#pragma once
#include <imgui/imgui.h>
#include <entt/entity/registry.hpp>
#include <nell/components/entity_name.hpp>

namespace nell::systems
{
template <typename Component>
void drawComponent(entt::registry &reg, entt::entity &entity)
{
  if (auto *comp = reg.try_get<Component>(entity); comp)
  {
    drawComponentImpl(*comp);
  }
}

template <typename... Component>
void drawEntityBrowser(entt::registry &reg)
{
  std::uint64_t dt;
  ImGui::Begin("Entity Browser");
  reg.each([&](auto entity) {
    auto name = std::string("Entity %d");
    if (auto *comp = reg.try_get<nell::comp::EntityName>(entity); comp)
    {
      name = comp->name;
    }
    if (ImGui::TreeNode((void *)(intptr_t)entt::to_integral(entity), name.c_str(),
                        entt::to_integral(entity)))
    {
      (drawComponent<Component>(reg, entity), ...);

      ImGui::TreePop();
    }
  });
  ImGui::End();
}

};  // namespace nell
