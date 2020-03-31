#pragma once
#include <glad/glad.h>

#include <entt/entity/registry.hpp>
#include <nell/input.hpp>
#include <nell/scene.hpp>
#include <nell/scenes/scene_impl.hpp>
#include <utils/gl_utils.hpp>
#include <utils/ping_pong_id.hpp>

namespace nell
{
class CullingCsScene final : public SceneImpl
{
 public:
  explicit CullingCsScene();
  void populate(Scene*, entt::registry& reg) override;
  void setup(Scene*, entt::registry& reg) override;
  void resize(int w, int h) override;
  void update(Scene* scene, entt::registry& reg,
              const input::NellInputList& input_list, const double& time,
              const double& delta_time) override;
  void render(Scene* scene, entt::registry& reg, entt::entity& camera_entity,
              const double& time, const double& delta_time) override;
  void drawSceneImGui(Scene* scene, entt::registry& reg) override;
  void drawStatsImGui(Scene* scene, entt::registry& reg, const double& time,
                      const double& delta_time) override;

 private:
  // Stats
  int _rendered_objects{};
  int _frame_count{};
  double _time_acc{};

  double _cpu_time{};
  double _cpu_time_min{DBL_MAX};
  double _cpu_time_max{};
  double _cpu_time_avg{};
  double _cpu_time_acc{};

  GLuint64 _gpu_time{};
  GLuint64 _gpu_time_min{UINT_MAX};
  GLuint64 _gpu_time_max{};
  GLuint64 _gpu_time_avg{};
  GLuint64 _gpu_time_acc{};

  GLuint _gpu_timer_query[2]{};
  utils::PingPongId<size_t> _swap_id;

  // Settings
  bool _disable_frustum_culling;
  bool _freeze_frustum_culling;
  bool _disable_backface_culling;
  bool _freeze_backface_culling;
  // Cached Data
  std::vector<glm::vec4> _cached_frustum_planes;
  glm::vec3 _cached_view_origin{};
  glm::vec3 _cached_view_up{};
  glm::vec3 _cached_view_right{};

  // Programs, Shader, Locations
  GLuint _program_pipeline[2]{};

  gl_utils::ShaderProgram _culling_compute_shader;
  GLuint _uniform_disable_frustum_culling_loc;
  GLuint _uniform_disable_backface_culling_loc;
  GLuint _uniform_view_origin_loc;
  /* // Needed for exact version of FrontFaceVisibility Test
  GLuint _uniform_view_up_loc;
  GLuint _uniform_view_right_loc;*/
  GLuint _uniform_frustum_planes_loc;
  GLuint _uniform_debug_value_loc;

  gl_utils::ShaderProgram _basic_vert_shader;
  GLuint _vertex_array_object;
  GLuint _position_attr_loc;
  GLuint _normal_attr_loc;
  GLuint _uniform_mvp_loc;
  GLuint _uniform_model_matrix_transpose_inverse;

  gl_utils::ShaderProgram _basic_frag_shader;
};

}  // namespace nell
