#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace nell::comp
{
struct Transform
{
  Transform();

  glm::vec3 getRight();
  glm::vec3 getUp();
  glm::vec3 getForward();
  glm::vec3 getScale();
  glm::vec3 getTranslation();
  glm::quat getRotation();
  glm::mat4 getTransformation();

  void setTranslation(glm::vec3 translation);
  void setRotation(glm::quat rotation);

 private:
  bool _is_dirty;
  glm::vec3 _scale;
  glm::quat _rotation;
  glm::vec3 _translation;
  glm::mat4 _transformation;
};

}  // namespace nell::comp