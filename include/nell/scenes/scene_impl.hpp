#pragma once
#include <entt/entity/registry.hpp>
#include <nell/input.hpp>

namespace nell
{
class Scene;

class SceneImpl
{
 public:
  virtual ~SceneImpl() = default;
  virtual void populate(Scene *, entt::registry &) = 0;
  virtual void setup(Scene *, entt::registry &) = 0;
  virtual void resize(int w, int h) = 0;
  virtual void update(Scene *scene, entt::registry &reg,
                      const input::NellInputList &input_list,
                      const double &time, const double &delta_time) = 0;
  virtual void render(Scene *scene, entt::registry &reg,
                      entt::entity &camera_entity, const double &time,
                      const double &delta_time) = 0;

  virtual void drawSceneImGui(Scene *scene, entt::registry &reg) = 0;
  virtual void drawStatsImGui(Scene *scene, entt::registry &reg,
                              const double &time, const double &delta_time) = 0;
};

}  // namespace nell
