#pragma once
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

namespace nell
{
typedef const std::function<void(entt::registry &)> SceneDefinitionFunction;

class Scene final
{
 public:
  ~Scene();
  // explicit Scene();
  explicit Scene(std::string, SceneDefinitionFunction &);

  void setTime(double);
  void setDeltaTime(double);
  std::string serialize();
  void deserialize(const std::string &archive = std::string());
  void update();
  void render();
  std::string getActiveScene() const;
  std::string getArchiveFileName() const;
  void setArchiveName(const std::string &);

 private:
  double _time;
  double _delta_time;
  std::string _scene_name;
  std::string _scene_archive_name;
  entt::registry _registry;

  void init();
};

}  // namespace nell
