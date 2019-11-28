#include <cereal/archives/json.hpp>
#include <nell/scene.hpp>
#include <utility>
#include "asset_import_system.hpp"
#include "shader_source.hpp"
#include "ui_entity_draw_system.hpp"

namespace nell
{
Scene::~Scene() = default;

/* TODO
 * - Toggable Entity Windows
 *
 */

Scene::Scene(std::string scene_name, SceneDefinitionFunction &scene_definition)
    : _time(0),
      _delta_time(0),
      _scene_name(std::move(scene_name)),
      _scene_archive_name("default_settings")
{
  scene_definition(_registry);
  init();
}


void Scene::setTime(const double time) { _time = time; }

void Scene::setDeltaTime(const double delta_time) { _delta_time = delta_time; }

std::string Scene::serialize()
{
  std::stringstream out_stream;
  {
    // archive flushes its contents when it goes out of scope
    cereal::JSONOutputArchive out_archive{out_stream};
    _registry.snapshot()
        .entities(out_archive)
        .component<AssetSourcePath, ShaderSource>(out_archive);
  }
  return out_stream.str();
}

void Scene::deserialize(const std::string &archive)
{
  if (!_registry.empty()) _registry.reset();
  std::stringstream in_stream(archive);
  cereal::JSONInputArchive in_archive{in_stream};

  _registry.loader()
      .entities(in_archive)
      .component<AssetSourcePath, ShaderSource>(in_archive);

  init();
}

void Scene::update()
{
  spdlog::debug("Update");
  updateEntityUi<AssetSourcePath, ShaderSource>(_registry);
}
void Scene::render() { spdlog::debug("Render"); }
std::string Scene::getActiveScene() const { return _scene_name; }

std::string Scene::getArchiveFileName() const
{
  return _scene_name + "_" + _scene_archive_name;
}

void Scene::setArchiveName(const std::string &archive_name)
{
  if (archive_name.empty())
  {
    _scene_archive_name = "DefaultSettings";
  } else
  {
    _scene_archive_name = archive_name;
  }
}

void Scene::init() { importAssets(_registry); }

}  // namespace nell