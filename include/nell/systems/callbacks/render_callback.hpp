#pragma once
#include <entt/entity/registry.hpp>

namespace nell
{
class RenderCallback
{
 public:
  virtual ~RenderCallback() = default;
  virtual void render(const double& time, const double& delta_time,
                      entt::registry& reg) = 0;
};

}  // namespace nell