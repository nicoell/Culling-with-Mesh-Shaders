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
class BasicScene final : public SceneImpl
{
 public:
  explicit BasicScene();
  void populate(Scene*, entt::registry& reg) override;
  void setup(Scene*, entt::registry& reg) override;
  void resize(int w, int h) override;
  void update(Scene* scene, entt::registry& reg,
              const input::NellInputList& input_list, const double& time,
              const double& delta_time) override;
  void render(Scene* scene, entt::registry& reg, entt::entity& camera_entity,
              const double& time, const double& delta_time) override;

  void drawSceneImGui (Scene *scene, entt::registry &reg) override;
  void drawStatsImGui (Scene *scene, entt::registry &reg, const double &time,
    const double &delta_time) override;
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
  utils::PingPongId <size_t> _swap_id;

  GLuint _program_pipeline[1];
  GLuint _vertex_array_object;

  gl_utils::ShaderProgram _basic_vert_shader;
  GLuint _position_attr_loc;
  GLuint _normal_attr_loc;
  GLuint _uniform_mvp_loc;
  GLuint _uniform_model_matrix_transpose_inverse;
  gl_utils::ShaderProgram _basic_fragment_shader;
};

}  // namespace nell
