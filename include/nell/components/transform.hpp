#pragma once
#include <glad.h>
#include <imgui/misc/imgui_stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <ui_drawable.hpp>

namespace nell::comp
{
struct Transform : UiDrawable
{
  static glm::quat getFromToRotation(glm::vec3 from, glm::vec3 to);

  Transform();
  Transform(const glm::vec3& translation, const glm::vec3& scale,
            const glm::quat& rotation);

  void setForward(const glm::vec3& forward);
  void setRight(const glm::vec3& right);
  void setUp(const glm::vec3& up);

  void setTranslation(const glm::vec3& translation);
  void setRotation(const float& angle, const glm::vec3& axis);
  void setRotation(const glm::vec3& euler_angles);
  void setRotation(const glm::quat& rotation);
  void setScale(const glm::vec3& scale);

  glm::vec3 getRight() const;
  glm::vec3 getUp() const;
  glm::vec3 getForward() const;

  glm::vec3 getTranslation() const;
  glm::quat getRotation() const;
  glm::vec3 getScale() const;

  glm::mat4 getTransformation();
  glm::mat4 getTransformationDirty() const;

  glm::vec3& getTranslationRef();
  glm::quat& getRotationRef();
  glm::vec3& getScaleRef();

  void setDirty(bool is_dirty) { _is_dirty = is_dirty; }
  bool isDirty() const { return _is_dirty; }
  const GLfloat* getTransformationValuePtr();
  const GLfloat* getTransformationDirtyValuePtr();

  inline static const glm::vec3 forward = glm::vec3(0, 0, 1);
  inline static const glm::vec3 up = glm::vec3(0, 1, 0);
  inline static const glm::vec3 right = glm::vec3(1, 0, 0);

  void drawImGui() override;

  bool operator==(const Transform& t2) const;
  bool operator==(const glm::mat4& t2) const;
  bool operator!=(const Transform& t2) const;
  bool operator!=(const glm::mat4& t2) const;

 private:
  void update();
  void updateEulerAngles();

  glm::vec3 _translation;
  glm::quat _rotation;
  glm::vec3 _scale;

  glm::mat4 _transformation;
  glm::vec3 _imgui_euler_angles;

  bool _is_dirty;

  inline static const float translation_epsilon = 1e-4f;
  inline static const float rotation_epsilon = 1e-6f;
  inline static const float scale_epsilon = 1e-4f;

  inline static const glm::quat up_rotation = glm::quatLookAt(up, up);
  inline static const glm::quat right_rotation = glm::quatLookAt(right, up);
};

/*template <typename Archive>
void serialize(Archive &archive, Transform &transform)
{
  //archive(transform.);
}*/


}  // namespace nell::comp