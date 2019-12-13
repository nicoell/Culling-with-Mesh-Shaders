#pragma once
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <scene_impl.hpp>
#include <input.hpp>

namespace nell
{
// typedef const std::function<void(entt::registry &)> SceneDefinitionFunction;

class Scene final
{
 public:
  ~Scene();
  // explicit Scene();
  explicit Scene(std::string, std::unique_ptr<SceneImpl>);

  std::string serialize() const;
  void deserialize(const std::string &archive = std::string());

  void update(const double& time, const double& delta_time, const input::NellInputList &);
  void render(const double& time, const double& delta_time);
  std::string getActiveScene() const;
  std::string getArchiveFileName() const;
  void setArchiveName(const std::string &);

  void resize(int w, int h);

 private:
  std::string _scene_name;
  std::string _scene_archive_name;
  entt::registry _registry;
  std::unique_ptr<SceneImpl> _scene_impl;

  entt::entity _camera;

  void init();
};

}  // namespace nell
