#pragma once
#include <glad/glad.h>

#include <nell/components/buffer_objects.hpp>
#include <nell/components/ui_drawable.hpp>

namespace nell::comp
{
// TODO: Refactor this together with mesh struct.

struct TriangleMeshDrawMeshtasksObjects final : BufferObjects, UiDrawable
{
  void drawImGui() override;
  GLuint vertex_ssbo;
  GLuint normal_ssbo;
  GLuint index_ssbo;
};

inline void TriangleMeshDrawMeshtasksObjects::drawImGui()
{
  ImGui::BulletText("TriangleMeshDrawMeshtasksObjects");
}

}  // namespace nell::comp
