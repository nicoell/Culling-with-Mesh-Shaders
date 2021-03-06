#pragma once

namespace nell::comp
{
template <class ComponentType>
struct RelationshipWorkable
{
  virtual ~RelationshipWorkable() = default;
  virtual void processWorkFromParent(ComponentType* parent_component) = 0;
};

}  // namespace nell::comp
