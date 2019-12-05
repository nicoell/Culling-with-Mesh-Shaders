#pragma once
#include <entt/entity/registry.hpp>
#include <input.hpp>

namespace nell
{
class SceneImpl
{
 public:
  virtual ~SceneImpl() = default;
  virtual void populate(entt::registry &) = 0;
  virtual void setup(entt::registry &) = 0;
  virtual void resize(int w, int h) = 0;
  virtual void update(const double &time, const double &delta_time,
                      const input::NellInputList &input_list,
                      entt::registry &reg) = 0;
  virtual void render(const double &time, const double &delta_time,
                      entt::registry &reg) = 0;
};

}  // namespace nell
