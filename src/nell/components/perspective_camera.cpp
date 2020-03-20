#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nell/components/perspective_camera.hpp>

namespace nell::comp
{
glm::mat4x4 PerspectiveCamera::getProjectionMatrix() const
{
  return _projection_matrix;
}

glm::mat4x4 PerspectiveCamera::getViewMatrix() const { return _view_matrix; }

glm::mat4x4 PerspectiveCamera::getViewProjectionMatrix() const
{
  return _view_projection_matrix;
}

const GLfloat* PerspectiveCamera::getProjectionMatrixValuePtr()
{
  return glm::value_ptr(_projection_matrix);
}

const GLfloat* PerspectiveCamera::getViewMatrixValuePtr()
{
  return glm::value_ptr(_view_matrix);
}

const GLfloat* PerspectiveCamera::getViewProjectionMatrixValuePtr()
{
  return glm::value_ptr(_view_projection_matrix);
}

void PerspectiveCamera::setFovY(const float fovy)
{
  _is_dirty |= glm::epsilonNotEqual(fovy, _fovy, fovy_epsilon);
  _fovy = fovy;
}
void PerspectiveCamera::setAspect(const float aspect)
{
  _is_dirty |= glm::epsilonNotEqual(aspect, _aspect, aspect_epsilon);
  _aspect = aspect;
}
void PerspectiveCamera::setAspect(const int w, const int h)
{
  float aspect = static_cast<float>(w) / static_cast<float>(h);
  _is_dirty |= glm::epsilonNotEqual(aspect, _aspect, aspect_epsilon);
  _aspect = aspect;
}
void PerspectiveCamera::setNearAndFar(const float nearplane,
                                      const float farplane)
{
  _is_dirty |= glm::epsilonNotEqual(nearplane, _nearplane, nearfar_epsilon);
  _is_dirty |= glm::epsilonNotEqual(farplane, _farplane, nearfar_epsilon);
  _nearplane = nearplane;
  _farplane = farplane;
}

void PerspectiveCamera::updateMatrices(comp::Transform& transform)
{
  if (_is_dirty)
  {
    _projection_matrix =
        glm::perspective(glm::radians(_fovy), _aspect, _nearplane, _farplane);

    _is_dirty = false;
  }
  auto eye = transform.getTranslation();
  auto center = eye + transform.getForward();
  // const auto up = glm::vec3(0, 1, 0);
  _view_matrix = glm::lookAt(eye, center, _up);
  //_view_matrix = glm::mat4_cast(transform.getRotationRef()) *
  //transform.getTranslationRef();
  _view_projection_matrix = _projection_matrix * _view_matrix;
}

void PerspectiveCamera::drawImGui()
{
  if (ImGui::TreeNode("Perspective Camera"))
  {
    float fovy = getFovY();
    float aspect = getAspect();
    float nearplane = getNearplane();
    float farplane = getFarplane();

    if (ImGui::SliderFloat("FOV Y", &fovy, 30.f, 90.0f))
    {
      setFovY(fovy);
    }
    ImGui::InputFloat("Aspect Ratio", &aspect, 0, 0, "%.3f",
                      ImGuiInputTextFlags_ReadOnly);
    if (ImGui::InputFloat("Near Plane", &nearplane) ||
        ImGui::InputFloat("Far Plane", &farplane))
    {
      setNearAndFar(nearplane, farplane);
    }
    ImGui::TreePop();
  }
}
}  // namespace nell::comp
