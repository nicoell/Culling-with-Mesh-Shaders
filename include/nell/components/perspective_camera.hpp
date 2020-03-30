#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <nell/components/transform.hpp>
#include <nell/components/ui_drawable.hpp>
#include <vector>

namespace nell::comp
{
struct PerspectiveCamera : UiDrawable
{
  PerspectiveCamera(float fovy = 60, float aspect = 1, float nearplane = 5,
                    float farplane = 1000)
      : _fovy(fovy),
        _meshletculling_fovy_rad(glm::radians(fovy)),
        _aspect(aspect),
        _nearplane(nearplane),
        _farplane(farplane),
        _up(0, 1, 0),
        _is_dirty(true),
        _view_matrix(1),
        _projection_matrix(1),
        _view_projection_matrix(1),
        _frustum_near_vertices(4),
        _frustum_far_vertices(4),
        _frustum_directions(4),
        _frustum_planes(6)
  {
    _frustum_planes[4] = glm::vec4(0, 0, -1, 0);
    _frustum_planes[5] = glm::vec4(0, 0, 1, 0);
  }

  float getFovY() const { return _fovy; }
  float getAspect() const { return _aspect; }
  float getNearplane() const { return _nearplane; }
  float getFarplane() const { return _farplane; }
  glm::vec3 getViewDir() const { return _view_dir; }
  const GLfloat* getViewDirValuePtr() const;
  std::vector<glm::vec4> getFrustumNearVertices() const
  {
    return _frustum_near_vertices;
  }
  std::vector<glm::vec4> getFrustumFarVertices() const
  {
    return _frustum_far_vertices;
  }
  std::vector<glm::vec3> getFrustumDirections() const
  {
    return _frustum_directions;
  }
  std::vector<glm::vec4> getFrustumPlanes() const
  {
    return _frustum_planes;
  }
  glm::mat4x4 getProjectionMatrix() const;
  glm::mat4x4 getViewMatrix() const;
  glm::mat4x4 getViewProjectionMatrix() const;
  const GLfloat* getProjectionMatrixValuePtr();
  const GLfloat* getViewMatrixValuePtr();
  const GLfloat* getViewProjectionMatrixValuePtr();
  bool isDirty() const { return _is_dirty; }

  void setFovY(float fovy);
  void setAspect(float aspect);
  void setAspect(int w, int h);
  void setNearAndFar(float nearplane, float farplane);

  void updateMatrices(comp::Transform&);

  void drawImGui() override;

 private:
  void calculateFrustumPoints();

  float _fovy_rad;
  float _meshletculling_fovy_rad;
  float _fovx_rad;

  float _fovy;
  float _aspect;
  float _nearplane;
  float _farplane;
  glm::vec3 _up;
  bool _is_dirty;

  glm::mat4x4 _view_matrix;
  glm::mat4x4 _projection_matrix;
  // glm::mat4x4 _inverse_projection_matrix;
  glm::mat4x4 _view_projection_matrix;
  glm::mat4x4 _inverse_view_projection_matrix;
  glm::vec3 _view_dir;
  // Frustum Data in World Space
  std::vector<glm::vec4> _frustum_near_vertices;  // tl,tr,br,bl
  std::vector<glm::vec4> _frustum_far_vertices;   // tl,tr,br,bl
  std::vector<glm::vec3> _frustum_directions;     // tl,tr,br,bl from near to
                                                  // far
  std::vector<glm::vec4> _frustum_planes;         // t,r,b,l,n,f

  inline static const float fovy_epsilon = 0.0001f;
  inline static const float aspect_epsilon = 0.0001f;
  inline static const float nearfar_epsilon = 0.0001f;
};

}  // namespace nell::comp
