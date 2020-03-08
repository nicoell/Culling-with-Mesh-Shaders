#include <components/transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace nell::comp
{
glm::quat Transform::getFromToRotation(glm::vec3 from, glm::vec3 to)
{
  // From Stan Melax "The Shortest Arc Quaternion" in Game Programming Gems 1,
  // p.214-218
  glm::quat q;

  from = glm::normalize(from);
  to = glm::normalize(to);

  float cos_theta = glm::dot(from, to);
  if (cos_theta >= 1.0f)
  {
    return glm::identity<glm::quat>();
  } else if (cos_theta < (1e-6f - 1.0f))
  {
    auto axis = glm::cross(glm::vec3(1, 0, 0), from);
    if (glm::length2(axis) == 0.0f)
    {
      axis = glm::cross(glm::vec3(0, 1, 0), from);
    }
    return glm::angleAxis(glm::pi<float>(), axis);
  }

  float s = glm::sqrt((1 + cos_theta) * 2);
  float invs = 1 / s;

  auto axis = glm::cross(from, to);
  return glm::quat(s * 0.5f, axis.x * invs, axis.y * invs, axis.z * invs);
}

Transform::Transform()
    : _translation(0),
      _rotation(),
      _scale(1),
      _transformation(1),
      _imgui_euler_angles(0, 0, 0),
      _is_dirty(true)
{
}

Transform::Transform(const glm::vec3& translation = glm::vec3(0),
                     const glm::vec3& scale = glm::vec3(1),
                     const glm::quat& rotation = glm::quat())
    : _translation(translation),
      _rotation(rotation),
      _scale(scale),
      _transformation(1),
      _imgui_euler_angles(0, 0, 0),
      _is_dirty(true)
{
}

void Transform::setForward(const glm::vec3& forward)
{
  setRotation(glm::quatLookAt(forward, Transform::up));
  _is_dirty = true;
}
void Transform::setRight(const glm::vec3& right)
{
  setRotation(Transform::getFromToRotation(Transform::right, right));
  _is_dirty = true;
}
void Transform::setUp(const glm::vec3& up)
{
  setRotation(Transform::getFromToRotation(Transform::up, up));
  _is_dirty = true;
}

void Transform::setTranslation(const glm::vec3& translation)
{
  _is_dirty |= glm::any(
      glm::epsilonNotEqual(_translation, translation, translation_epsilon));
  _translation = translation;
}

void Transform::setRotation(const glm::quat& rotation)
{
  if (glm::any(glm::epsilonNotEqual(_rotation, rotation, rotation_epsilon)))
  {
    _is_dirty = true;
    _rotation = rotation;
    updateEulerAngles();
  }
}

void Transform::setRotation(const glm::vec3& euler_angles)
{
  if (glm::any(glm::epsilonNotEqual(_imgui_euler_angles, euler_angles,
                                    rotation_epsilon)))
  {
    _is_dirty = true;
    _rotation = glm::quat(euler_angles);
    _imgui_euler_angles = euler_angles;
  }
}

void Transform::setScale(const glm::vec3& scale)
{
  _is_dirty |= glm::any(glm::epsilonNotEqual(_scale, scale, scale_epsilon));
  _scale = scale;
}

glm::vec3 Transform::getRight() const { return _rotation * Transform::right; }
glm::vec3 Transform::getUp() const { return _rotation * Transform::up; }
glm::vec3 Transform::getForward() const
{
  return _rotation * Transform::forward;
}
glm::vec3 Transform::getScale() const { return _scale; }
glm::vec3& Transform::getTranslationRef() { return _translation; }
glm::quat& Transform::getRotationRef() { return _rotation; }
glm::vec3& Transform::getScaleRef() { return _scale; }
glm::vec3 Transform::getTranslation() const { return _translation; }
glm::quat Transform::getRotation() const { return _rotation; }

glm::mat4 Transform::getTransformation()
{
  update();
  return _transformation;
}

glm::mat4 Transform::getTransformationDirty() const { return _transformation; }

const GLfloat* Transform::getTransformationValuePtr()
{
  update();
  return glm::value_ptr(_transformation);
}

const GLfloat* Transform::getTransformationDirtyValuePtr()
{
  return glm::value_ptr(_transformation);
}

void Transform::drawImGui()
{
  glm::vec3 translation = _translation;
  glm::vec3 scale = _scale;
  glm::quat rotation = _rotation;
  glm::vec3 euler_angles = glm::degrees(_imgui_euler_angles);
  glm::vec3 forward = getForward();
  glm::vec3 right = getRight();
  glm::vec3 up = getUp();
  if (ImGui::DragFloat3("Translation", glm::value_ptr(translation), 0.2f))
  {
    setTranslation(translation);
  }
  if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.05f))
  {
    setScale(scale);
  }
  if (ImGui::DragFloat4("Quaternion", glm::value_ptr(rotation), 0.01f, -1.f,
                        1.f))
  {
    rotation = glm::normalize(rotation);
    setRotation(rotation);
  }
  if (ImGui::DragFloat3("Euler Angles", glm::value_ptr(euler_angles)))
  {
    setRotation(glm::radians(euler_angles));
  }
  if (ImGui::DragFloat3("Forward", glm::value_ptr(forward), 0.01f, -1.f, 1.f))
  {
    forward = glm::normalize(forward);
    setForward(forward);
  }
  if (ImGui::DragFloat3("Right", glm::value_ptr(right), 0.01f, -1.f, 1.f))
  {
    right = glm::normalize(right);
    setRight(right);
  }
  if (ImGui::DragFloat3("Up", glm::value_ptr(up), 0.01f, -1.f, 1.f))
  {
    up = glm::normalize(up);
    setUp(up);
  }
}

void Transform::update()
{
  if (_is_dirty)
  {
    // Cumulate translation, rotation and scale
    auto mat = glm::translate(_translation);
    mat = mat * glm::toMat4(_rotation);
    _transformation = glm::scale(mat, _scale);
    _is_dirty = false;
  }
}

void Transform::updateEulerAngles()
{
  _imgui_euler_angles = glm::eulerAngles(_rotation);
}

}  // namespace nell::comp
