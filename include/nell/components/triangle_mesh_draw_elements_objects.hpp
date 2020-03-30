#pragma once
#include <glad/glad.h>

#include <nell/components/buffer_objects.hpp>
#include <nell/components/ui_drawable.hpp>

namespace nell::comp
{
struct TriangleMeshDrawElementsObjects final : BufferObjects, UiDrawable
{
  void drawImGui() override;
  GLuint vertex_buffer_object;
  GLuint normal_buffer_object;
  GLuint index_buffer_object;
};

inline void TriangleMeshDrawElementsObjects::drawImGui()
{
  if (ImGui::TreeNode("Meshlet Triangle Mesh"))
  {
    ImGui::BulletText("TriangleMeshDrawElementsObjects");
    ImGui::TreePop();
  }
}

}  // namespace nell::comp
