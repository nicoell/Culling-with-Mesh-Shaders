#pragma once
#include <entt/entity/registry.hpp>
#include <input.hpp>
#include <scene.hpp>

namespace nell
{
class SceneImpl
{
 public:
  virtual ~SceneImpl() = default;
  virtual void populate(Scene *, entt::registry &) = 0;
  virtual void setup(Scene *, entt::registry &) = 0;
  /*virtual void resize(int w, int h) = 0;
  virtual void update(const double &time, const double &delta_time,
                      const input::NellInputList &input_list,
                      entt::registry &reg) = 0;
  virtual void render(const double &time, const double &delta_time,
                      entt::registry &reg) = 0;*/
};

}  // namespace nell
