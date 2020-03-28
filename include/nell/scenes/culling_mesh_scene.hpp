#pragma once
#include <glad/glad.h>

#include <entt/entity/registry.hpp>
#include <nell/input.hpp>
#include <nell/scene.hpp>
#include <nell/scenes/scene_impl.hpp>
#include <utils/gl_utils.hpp>

namespace nell
{
/*
 * Cluster Culling techniques
    Backface Culling
        Optimal Bounding Cone of triangle normals
         - Cull if dot(cone.Normal, -view) < -sin(cone.angle)
    Frustum Culling
        Bounding Sphere
    Occlusion Culling
        Hierarchical Z Map with Bounding Sphere
        Depth pre-pass with best Occluders
 */
class CullingMeshScene final : public SceneImpl
{
 public:
  explicit CullingMeshScene();
  void populate(Scene*, entt::registry& reg) override;
  void setup(Scene*, entt::registry& reg) override;
  void resize(int w, int h) override;
  void update(Scene* scene, entt::registry& reg,
              const input::NellInputList& input_list, const double& time,
              const double& delta_time) override;
  void render(Scene* scene, entt::registry& reg, entt::entity& camera_entity,
              const double& time, const double& delta_time) override;

 private:
  GLuint _program_pipeline[1];
  GLuint _vertex_array_object;

  gl_utils::ShaderProgram _culling_task_shader;
  GLuint _uniform_view_origin_loc;
  /* // Needed for exact version of FrontFaceVisibility Test
  GLuint _uniform_view_up_loc; 
  GLuint _uniform_view_right_loc;*/
  GLuint _uniform_frustum_planes_loc;
  GLuint _uniform_debug_value_loc;

  gl_utils::ShaderProgram _culling_mesh_shader;
  GLuint _uniform_mvp_loc;
  GLuint _uniform_model_matrix_transpose_inverse;

  gl_utils::ShaderProgram _culling_fragment_shader;

  std::vector<std::map<const char*, GLuint>> _resource_location_maps;
};

}  // namespace nell
