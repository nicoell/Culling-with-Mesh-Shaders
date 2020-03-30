#pragma once
#include <entt/entt.hpp>
#include <nell/components/freeflight_controller.hpp>
#include <nell/components/model_source.hpp>
#include <nell/components/perspective_camera.hpp>
#include <nell/components/transform.hpp>
#include <nell/input.hpp>
#include <nell/scenes/scene_impl.hpp>
#include <nell/systems/relationship_work_system.hpp>
#include <nell/systems/ui_entity_draw_system.hpp>

namespace nell
{
class Scene final
{
 public:
  ~Scene();
  explicit Scene(std::string, std::unique_ptr<SceneImpl>, int, int);

  std::string serialize() const;
  void deserialize(const std::string &archive = std::string());

  template <typename... Component>
  void drawComponentImGui();
  void drawStatsImGui(const double &time, const double &delta_time);
  void update(const double &time, const double &delta_time,
              const input::NellInputList &);
  void render(const double &time, const double &delta_time);
  std::string getActiveScene() const;
  std::string getArchiveFileName() const;
  void setArchiveName(const std::string &);

  void resize(int w, int h);

 private:
  std::string _scene_name;
  std::string _scene_archive_name;
  entt::registry _registry;
  std::unique_ptr<SceneImpl> _scene_impl;
  systems::RelationshipProcessor _relationship_processor;

  int _width;
  int _height;

  entt::entity _camera;

  void init();
};

template <typename... Component>
void Scene::drawComponentImGui()
{
  systems::drawEntityComponentHierarchyUi<
      comp::Transform, comp::FreeflightController, comp::PerspectiveCamera,
      comp::ModelSource, Component...>(_registry, "Hierarchy Browser");
}
}  // namespace nell
