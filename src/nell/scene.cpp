#include <cereal/archives/json.hpp>
#include <nell/components/freeflight_controller.hpp>
#include <nell/components/perspective_camera.hpp>
#include <nell/components/transform.hpp>
#include <nell/scene.hpp>
#include <nell/systems/camera_system.hpp>
#include <nell/systems/freeflight_controller_system.hpp>
#include <nell/systems/model_import_system.hpp>
#include <utility>

namespace nell
{
Scene::~Scene() = default;

Scene::Scene(std::string scene_name, std::unique_ptr<SceneImpl> scene_impl,
             int width, int height)
    : _scene_name(std::move(scene_name)),
      _scene_archive_name("default_settings"),
      _scene_impl(std::move(scene_impl)),
      _width(width),
      _height(height)
{
  _scene_impl->populate(this, _registry);
  init();
  _scene_impl->setup(this, _registry);
}

std::string Scene::serialize() const
{
  std::stringstream out_stream;
  {
    // archive flushes its contents when it goes out of scope
    cereal::JSONOutputArchive out_archive{out_stream};
    _registry.snapshot()
        .entities(out_archive)
        .component<comp::ModelSource>(out_archive);
  }
  return out_stream.str();
}

void Scene::deserialize(const std::string &archive)
{
  if (!_registry.empty()) _registry.clear();
  std::stringstream in_stream(archive);
  cereal::JSONInputArchive in_archive{in_stream};

  _registry.loader()
      .entities(in_archive)
      .component<comp::ModelSource>(in_archive);

  init();
}

void Scene::resize(int w, int h)
{
  auto perspective_camera = _registry.get<comp::PerspectiveCamera>(_camera);
  _width = w;
  _height = h;
  perspective_camera.setAspect(w, h);
  _scene_impl->resize(w, h);
}

void Scene::update(const double &time, const double &delta_time,
                   const input::NellInputList &input_list)
{
  systems::updateFreeflightController(_registry, _camera, input_list,
                                      delta_time);
  systems::updatePerspectiveCamera(_registry, _camera);

  _scene_impl->update(this, _registry, input_list, time, delta_time);

  _relationship_processor.processRelationshipWorkTopDown<comp::Transform>(
      _registry);
}
void Scene::render(const double &time, const double &delta_time)
{
  _scene_impl->render(this, _registry, _camera, time, delta_time);
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
  //_registry.on_construct<double>().connect<&entt::registry::assign_or_replace<int>>();
  //_registry.on_construct<comp::ModelSource>().connect<&entt::registry::assign_or_replace<comp::Transform>>();

  _camera = _registry.create();
  _registry.assign<comp::EntityName>(_camera, "MainCamera");
  auto &tf = _registry.assign<comp::Transform>(_camera);
  auto &ffc = _registry.assign<comp::FreeflightController>(_camera);
  auto &pc = _registry.assign<comp::PerspectiveCamera>(_camera);

  pc.setAspect(_width, _height);

  // systems::importAssets(_registry);
}
}  // namespace nell
