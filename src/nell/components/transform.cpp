#include <components/transform.hpp>

namespace nell::comp
{
Transform::Transform()
    : _is_dirty(false),
      _scale(1),
      _rotation(),
      _translation(0),
      _transformation(1)
{
}

glm::vec3 Transform::getRight()
{
  auto transformation = getTransformation();
  return {transformation[0][0], transformation[0][1], transformation[0][2]};
}
glm::vec3 Transform::getUp()
{
  auto transformation = getTransformation();
  return {transformation[1][0], transformation[1][1], transformation[1][2]};
}
glm::vec3 Transform::getForward()
{
  auto transformation = getTransformation();
  return {transformation[2][0], transformation[2][1], transformation[2][2]};
}
glm::vec3 Transform::getScale() { return _scale; }
glm::vec3 Transform::getTranslation() { return _translation; }
glm::quat Transform::getRotation() { return _rotation; }
glm::mat4 Transform::getTransformation()
{
  if (_is_dirty)
  {
    // Cumulate translation, rotation and scale
    auto mat = glm::translate(_translation);
    mat = mat * glm::toMat4(_rotation);
    _transformation = glm::scale(mat, _scale);
    _is_dirty = false;
  }
  return _transformation;
}

void Transform::setTranslation(glm::vec3 translation)
{
  _is_dirty = true;
  _translation = translation;
}

void Transform::setRotation(glm::quat rotation)
{
  _is_dirty = true;
  _rotation = rotation;
}

}  // namespace nell::comp
