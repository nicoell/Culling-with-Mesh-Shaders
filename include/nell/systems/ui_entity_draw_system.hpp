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
    comp->drawImGui();
    // drawComponentImpl(*comp);
  }
}

template <typename... Component>
void drawChildren(entt::registry &reg, entt::entity &entity)
{
  if (auto *child_link = reg.try_get<comp::ChildLink>(entity); child_link)
  {
    for (auto child : child_link->children)
    {
      auto name = std::string("Entity %d");
      if (comp::EntityName *comp = reg.try_get<comp::EntityName>(child); comp)
      {
        name = comp->name;
      }
      if (ImGui::TreeNode((void *)(intptr_t)entt::to_integral(child),
                          name.c_str(), entt::to_integral(child)))
      {
        (drawComponent<Component>(reg, child), ...);
        drawChildren<Component...>(reg, child);

        ImGui::TreePop();
      }
    }
  }
}

template <typename... Component>
void drawEntityComponentHierarchyUi(entt::registry &reg, const char *window_name)
{
  std::uint64_t dt;
  ImGui::Begin(window_name);
  reg.view<comp::EntityName>(entt::exclude<comp::ParentLink>)
      .each([&](auto entity, auto &entity_name) {
        if (ImGui::TreeNode((void *)(intptr_t)entt::to_integral(entity),
                            entity_name.name.c_str(),
                            entt::to_integral(entity)))
        {
          (drawComponent<Component>(reg, entity), ...);
          drawChildren<Component...>(reg, entity);

          ImGui::TreePop();
        }
      });
  ImGui::End();
}

template <typename... Component>
void drawEntityComponentListUi(entt::registry &reg, const char *window_name)
{
  std::uint64_t dt;
  ImGui::Begin(window_name);
  reg.each([&](auto entity) {
    auto name = std::string("Entity %d");
    if (comp::EntityName *comp = reg.try_get<comp::EntityName>(entity); comp)
    {
      name = comp->name;
    }
    if (ImGui::TreeNode((void *)(intptr_t)entt::to_integral(entity),
                        name.c_str(), entt::to_integral(entity)))
    {
      (drawComponent<Component>(reg, entity), ...);

      ImGui::TreePop();
    }
  });
  ImGui::End();
}

};  // namespace nell::systems
