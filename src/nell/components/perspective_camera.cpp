#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nell/components/perspective_camera.hpp>

namespace nell::comp
{
const GLfloat* PerspectiveCamera::getViewDirValuePtr() const
{
  return glm::value_ptr(_view_dir);
}

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
    _fovy_rad = glm::radians(_fovy);
    _fovx_rad = 2 * atan(_aspect * tan(0.5 * _fovy_rad));
    _projection_matrix =
        glm::perspective(_fovy_rad, _aspect, _nearplane, _farplane);

    _is_dirty = false;
  }
  auto eye = transform.getTranslation();
  _view_dir = transform.getForward();
  auto center = eye + _view_dir;
  // const auto up = glm::vec3(0, 1, 0);
  _view_matrix = glm::lookAt(eye, center, _up);
  //_view_matrix = glm::mat4_cast(transform.getRotationRef()) *
  // transform.getTranslationRef();
  _view_projection_matrix = _projection_matrix * _view_matrix;
  _inverse_view_projection_matrix = glm::inverse(_view_projection_matrix);
  calculateFrustumPoints();
}

void PerspectiveCamera::drawImGui()
{
  if (ImGui::TreeNode("Perspective Camera"))
  {
    float fovy = getFovY();
    float meshlet_fovy = glm::degrees(_meshletculling_fovy_rad);
    float aspect = getAspect();
    float nearplane = getNearplane();
    float farplane = getFarplane();

    if (ImGui::SliderFloat("FOV Y", &fovy, 30.f, 90.0f))
    {
      setFovY(fovy);
    }
    if (ImGui::InputFloat("Aspect Ratio", &aspect, 0, 0, "%.3f"))
    {
      setAspect(aspect);
    };
    if (ImGui::InputFloat("Near Plane", &nearplane) ||
        ImGui::InputFloat("Far Plane", &farplane))
    {
      setNearAndFar(nearplane, farplane);
    }

    if (ImGui::SliderFloat("Culling FOV Y", &meshlet_fovy, 30.f, 90.0f))
    {
      _meshletculling_fovy_rad = glm::radians(meshlet_fovy);
    }

    ImGui::TreePop();
  }
}

void PerspectiveCamera::calculateFrustumPoints()
{
  float half_height_one = glm::tan(_meshletculling_fovy_rad * 0.5f);

  {
    float half_height_near = half_height_one * _nearplane;
    float half_width_near = half_height_near * _aspect;

    _frustum_near_vertices[0] =
        glm::vec4(-half_width_near, half_height_near, _nearplane, 1);
    _frustum_near_vertices[1] =
        glm::vec4(half_width_near, half_height_near, _nearplane, 1);
    _frustum_near_vertices[2] =
        glm::vec4(half_width_near, -half_height_near, _nearplane, 1);
    _frustum_near_vertices[3] =
        glm::vec4(-half_width_near, -half_height_near, _nearplane, 1);
  }

  {
    float half_height_far = half_height_one * _farplane;
    float half_width_far = half_height_far * _aspect;

    _frustum_far_vertices[0] =
        glm::vec4(-half_width_far, half_height_far, _farplane, 1);
    _frustum_far_vertices[1] =
        glm::vec4(half_width_far, half_height_far, _farplane, 1);
    _frustum_far_vertices[2] =
        glm::vec4(half_width_far, -half_height_far, _farplane, 1);
    _frustum_far_vertices[3] =
        glm::vec4(-half_width_far, -half_height_far, _farplane, 1);
  }

  // tl,tr,br,bl
  for (int i = 0; i < 4; ++i)
  {
    _frustum_directions[i] = glm::normalize(_frustum_far_vertices[i]);
  }

  // t, r, b, l, n, f
  _frustum_planes[0] =
      glm::vec4(glm::cross(_frustum_directions[0], _frustum_directions[1]), 0);
  _frustum_planes[0].w =
      -glm::dot(_frustum_planes[0], _frustum_near_vertices[0]);

  _frustum_planes[1] =
      glm::vec4(glm::cross(_frustum_directions[1], _frustum_directions[2]), 0);
  _frustum_planes[1].w =
      -glm::dot(_frustum_planes[1], _frustum_near_vertices[1]);

  _frustum_planes[2] =
      glm::vec4(glm::cross(_frustum_directions[2], _frustum_directions[3]), 0);
  _frustum_planes[2].w =
      -glm::dot(_frustum_planes[2], _frustum_near_vertices[2]);

  _frustum_planes[3] =
      glm::vec4(glm::cross(_frustum_directions[3], _frustum_directions[0]), 0);
  _frustum_planes[3].w =
      -glm::dot(_frustum_planes[3], _frustum_near_vertices[3]);

  // near and far plane normals are constant (see constructor)
  // but update plane d
  _frustum_planes[4].w = 0;
  _frustum_planes[4].w =
      -glm::dot(_frustum_planes[4], _frustum_near_vertices[0]);

  _frustum_planes[5].w = 0;
  _frustum_planes[5].w =
      -glm::dot(_frustum_planes[5], _frustum_far_vertices[0]);
}
}  // namespace nell::comp
