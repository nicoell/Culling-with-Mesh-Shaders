#pragma once
#include <spdlog/spdlog.h>

#include <algorithm>
#include <any>
#include <callbacks/early_update_callback.hpp>
#include <callbacks/fixed_update_callback.hpp>
#include <callbacks/late_update_callback.hpp>
#include <callbacks/post_render_callback.hpp>
#include <callbacks/render_callback.hpp>
#include <callbacks/update_callback.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <input.hpp>
#include <scene_impl.hpp>
#include <callbacks/early_update_emitter.hpp>
#include <callbacks/update_emitter.hpp>
#include <callbacks/render_emitter.hpp>

namespace nell
{
// using EarlyUpdateCallback =
//    std::function<void(const double&, const double&, entt::registry&)>;
// using UpdateCallback =
//    std::function<void(const double&, const double&, entt::registry&)>;
// using LateUpdateCallback =
//    std::function<void(const double&, const double&, entt::registry&)>;
// using FixedUpdateCallback =
//    std::function<void(const double&, const double&, entt::registry&)>;
// using RenderCallback =
//    std::function<void(const double&, const double&, entt::registry&)>;
// using PostRenderCallback =
//    std::function<void(const double&, const double&, entt::registry&)>;
//
// using ResizeCallback = std::function<void(int& w, int& h)>;
//
// enum SceneCallbackEnum
//{
//  kEarlyUpdateCallbackEnum,
//  kUpdateCallbackEnum,
//  kLateUpdateCallbackEnum,
//  kFixedUpdateCallbackEnum,
//  kRenderCallbackEnum,
//  kPostRenderCallbackEnum,
//  kResizeCallbackEnum
//};

class Scene final
{
 public:
  ~Scene();
  // explicit Scene();
  explicit Scene(std::string, std::unique_ptr<SceneImpl>);

  std::string serialize() const;
  void deserialize(const std::string &archive = std::string());

  void update(const double &time, const double &delta_time,
              const input::NellInputList &);
  void render(const double &time, const double &delta_time);
  std::string getActiveScene() const;
  std::string getArchiveFileName() const;
  void setArchiveName(const std::string &);

  void resize(int w, int h);

  // template <typename T>
  // unsigned registerCallback(SceneCallbackEnum callback_enum, T callback);
  // template <typename T>
  // void unregisterCallback(SceneCallbackEnum callback_enum, unsigned handle);

  // unsigned addEarlyUpdateCallback(EarlyUpdateCallback callback);
  // unsigned addUpdateCallback(UpdateCallback callback);
  // unsigned addLateUpdateCallback(LateUpdateCallback callback);
  // unsigned addFixedUpdateCallback(FixedUpdateCallback callback);
  // unsigned addRenderCallback(RenderCallback callback);
  // unsigned addPostRenderCallback(PostRenderCallback callback);

  // void removeEarlyUpdateCallback(unsigned handle);
  // void removeUpdateCallback(unsigned handle);
  // void removeLateUpdateCallback(unsigned handle);
  // void removeFixedUpdateCallback(unsigned handle);
  // void removeRenderCallback(unsigned handle);
  // void removePostRenderCallback(unsigned handle);

 private:
  std::string _scene_name;
  std::string _scene_archive_name;
  entt::registry _registry;
  std::unique_ptr<SceneImpl> _scene_impl;

  entt::entity _camera;

  // template <typename T>
  // static unsigned addCallback(std::vector<CallbackFuncHandle<T>>& callbacks,
  //                            T callback);
  // template <typename T>
  // static void removeCallback(std::vector<CallbackFuncHandle<T>>& callbacks,
  //                           unsigned handle);

  // std::map<SceneCallbackEnum, std::any> _callbacks_map;
  // std::vector<CallbackFuncHandle<EarlyUpdateCallback>>
  // _early_update_callbacks; std::vector<CallbackFuncHandle<UpdateCallback>>
  // _update_callbacks; std::vector<CallbackFuncHandle<LateUpdateCallback>>
  // _late_update_callbacks;
  // std::vector<CallbackFuncHandle<FixedUpdateCallback>>
  // _fixed_update_callbacks; std::vector<CallbackFuncHandle<RenderCallback>>
  // _render_callbacks; std::vector<CallbackFuncHandle<PostRenderCallback>>
  // _post_render_callbacks;

  void init();
};
//
// template <typename T>
// unsigned Scene::registerCallback(SceneCallbackEnum callback_enum, T callback)
//{
//  return addCallback(std::any_cast<std::vector<CallbackFuncHandle<T>>>(
//                         _callbacks_map[callback_enum]),
//                     callback);
//}
//
// template<typename T>
// void Scene::unregisterCallback (SceneCallbackEnum callback_enum,
//  unsigned handle)
//{
//  removeCallback(std::any_cast<std::vector<CallbackFuncHandle<T>>>(
//                         _callbacks_map[callback_enum]),
//                     handle);
//}
//
// template <typename T>
// unsigned Scene::addCallback(std::vector<CallbackFuncHandle<T>>& callbacks,
//                            T callback)
//{
//  unsigned handle = (callbacks.empty()) ? 0 : callbacks.back().handle + 1;
//
//  callbacks.emplace_back(handle, callback);
//  return handle;
//}
//
// template <typename T>
// void Scene::removeCallback(std::vector<CallbackFuncHandle<T>>& callbacks,
//                           unsigned handle)
//{
//  auto pos = std::find(callbacks.begin(), callbacks.end(), handle);
//  if (pos != callbacks.end()) callbacks.erase(pos);
//}
}  // namespace nell
