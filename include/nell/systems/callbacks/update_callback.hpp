#pragma once

#include <entt/entity/registry.hpp>

namespace nell
{
class UpdateCallback
{
 public:
  virtual ~UpdateCallback() = default;
  virtual void update(const double& time, const double& delta_time,
                      entt::registry& reg) = 0;
};

}  // namespace nell