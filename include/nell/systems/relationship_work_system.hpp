#pragma once
#include <entt/entity/registry.hpp>
#include <nell/components/child_parent_relationship.hpp>
#include <queue>

namespace nell::systems
{
class RelationshipProcessor
{
 public:
  template <typename... WorkComponent>
  void processRelationshipWorkTopDown(entt::registry &reg)
  {
    auto root_view = reg.view<comp::ChildLink>(entt::exclude<comp::ParentLink>);
    auto child_view = reg.view<comp::ChildLink>();
    auto parent_view = reg.view<comp::ParentLink>();

    for (auto entity : root_view)
    {
      auto child_link = root_view.get<comp::ChildLink>(entity);

      (processWorkComponenentTopDown<WorkComponent>(reg, entity), ...);

      for (auto child : child_link.children)
      {
        _entity_queue_a.push(child);
      }
    }

    while (!_entity_queue_a.empty() || !_entity_queue_b.empty())
    {
      while (!_entity_queue_a.empty())
      {
        auto entity = _entity_queue_a.front();
        auto parent_entity = parent_view.get<comp::ParentLink>(entity).parent;

        (processWorkComponenentTopDown<WorkComponent>(reg, entity,
                                                      parent_entity),
         ...);

        if (auto *child_link = reg.try_get<comp::ChildLink>(entity); child_link)
        {
          for (auto child : child_link->children)
          {
            _entity_queue_b.push(child);
          }
        }

        _entity_queue_a.pop();
      }
      while (!_entity_queue_b.empty())
      {
        auto entity = _entity_queue_b.front();
        auto parent_entity = parent_view.get<comp::ParentLink>(entity).parent;

        (processWorkComponenentTopDown<WorkComponent>(reg, entity,
                                                      parent_entity),
         ...);

        if (auto *child_link = reg.try_get<comp::ChildLink>(entity); child_link)
        {
          for (auto child : child_link->children)
          {
            _entity_queue_a.push(child);
          }
        }

        _entity_queue_b.pop();
      }
    }
  }

 private:
  template <typename WorkComponent>
  void processWorkComponenentTopDown(entt::registry &reg, entt::entity &entity,
                                     entt::entity &parent) const
  {
    if (auto *comp = reg.try_get<WorkComponent> (entity); comp)
    {
      if (auto *comp_parent = reg.try_get<WorkComponent> (parent);
          !comp_parent)
      {
        comp->processWorkFromParent(nullptr);
      } else
      {
        comp->processWorkFromParent(comp_parent);
      }
    }
  }

  template <typename WorkComponent>
  void processWorkComponenentTopDown(entt::registry &reg,
                                     entt::entity &entity) const
  {
    if (auto *comp = reg.try_get<WorkComponent> (entity); comp)
    {
      comp->processWorkFromParent(nullptr);
    }
  }

  std::queue<entt::entity> _entity_queue_a;
  std::queue<entt::entity> _entity_queue_b;
};

}  // namespace nell::systems
