#pragma once
#include <glad.h>
#include <imgui/misc/imgui_stdlib.h>

#include <glm/glm.hpp>
#include <transform.hpp>

namespace nell::comp
{
struct PerspectiveCamera
{
  PerspectiveCamera(float fovy = 60, float aspect = 1, float nearplane = 5,
                    float farplane = 1000)
      : _fovy(fovy),
        _aspect(aspect),
        _nearplane(nearplane),
        _farplane(farplane),
        _up(0, 1, 0),
        _is_dirty(true),
        _view_matrix(1),
        _projection_matrix(1),
        _view_projection_matrix(1)
  {
  }

  float getFovY() const { return _fovy; }
  float getAspect() const { return _aspect; }
  float getNearplane() const { return _nearplane; }
  float getFarplane() const { return _farplane; }
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

 private:
  float _fovy;
  float _aspect;
  float _nearplane;
  float _farplane;
  glm::vec3 _up;
  bool _is_dirty;

  glm::mat4x4 _view_matrix;
  glm::mat4x4 _projection_matrix;
  glm::mat4x4 _view_projection_matrix;

  inline static const float fovy_epsilon = 0.0001f;
  inline static const float aspect_epsilon = 0.0001f;
  inline static const float nearfar_epsilon = 0.0001f;
};

inline void drawComponentImpl(PerspectiveCamera& perspective_camera)
{
  if (ImGui::TreeNode("Perspective Camera"))
  {
    float fovy = perspective_camera.getFovY();
    float aspect = perspective_camera.getAspect();
    float nearplane = perspective_camera.getNearplane();
    float farplane = perspective_camera.getFarplane();

    if (ImGui::SliderFloat("FOV Y", &fovy, 30.f, 90.0f))
    {
      perspective_camera.setFovY(fovy);
    }
    ImGui::InputFloat("Aspect Ratio", &aspect, 0, 0, "%.3f",
                      ImGuiInputTextFlags_ReadOnly);
    if (ImGui::InputFloat("Near Plane", &nearplane) ||
        ImGui::InputFloat("Far Plane", &farplane))
    {
      perspective_camera.setNearAndFar(nearplane, farplane);
    }
    ImGui::TreePop();
  }
}
}  // namespace nell::comp
