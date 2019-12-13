#include <cereal/archives/json.hpp>
#include <nell/components/mesh.hpp>
#include <nell/components/shaders.hpp>
#include <nell/scene.hpp>
#include <nell/systems/model_import_system.hpp>
#include <nell/systems/ui_entity_draw_system.hpp>
#include <utility>
#include <freeflight_camera_system.hpp>
#include <freeflight_camera.hpp>
#include <transform.hpp>

namespace nell
{
Scene::~Scene() = default;

/* TODO
 * - Toggable Entity Windows
 *
 */

Scene::Scene(std::string scene_name, std::unique_ptr<SceneImpl> scene_impl)
    : _scene_name(std::move(scene_name)),
      _scene_archive_name("default_settings"),
      _scene_impl(std::move(scene_impl))
{
  _scene_impl->populate(_registry);
  init();
  _scene_impl->setup(_registry);
}

std::string Scene::serialize() const
{
  std::stringstream out_stream;
  {
    // archive flushes its contents when it goes out of scope
    cereal::JSONOutputArchive out_archive{out_stream};
    _registry.snapshot()
        .entities(out_archive)
        .component<comp::ModelSource, Shaders>(out_archive);
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
      .component<comp::ModelSource, Shaders>(in_archive);

  init();
}

void Scene::resize(int w, int h) { _scene_impl->resize(w, h); }

void Scene::update(const double &time, const double &delta_time,
                   const input::NellInputList &input_list)
{
  spdlog::debug("Update");
  systems::drawEntityBrowser<comp::ModelSource, comp::Model, Shaders>(_registry);
  systems::updateFreeflightCamera(_registry, _camera, input_list, delta_time);

  _scene_impl->update(time, delta_time, input_list, _registry);
}
void Scene::render(const double &time, const double &delta_time)
{
  spdlog::debug("Render");
  _scene_impl->render(time, delta_time, _registry);
}
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

void Scene::init()
{
  _camera = _registry.create();
  auto &tf = _registry.assign<comp::Transform>(_camera);
  auto &ffc = _registry.assign<comp::FreeflightCamera>(_camera);
  systems::importAssets(_registry);
}

}  // namespace nell