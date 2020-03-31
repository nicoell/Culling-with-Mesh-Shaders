#pragma once
#include <glad/glad.h>

#include <nell/components/buffer_objects.hpp>
#include <nell/components/ui_drawable.hpp>

namespace nell::comp
{
struct DrawElementsIndirectCommand
{
  alignas(4) GLuint count;
  alignas(4) GLuint instanceCount;
  alignas(4) GLuint firstIndex;
  alignas(4) GLuint baseVertex;
  alignas(4) GLuint baseInstance;
};

struct MeshletTriangleMeshCsObjects final : BufferObjects, UiDrawable
{
  void drawImGui() override;
  GLuint vertex_bo;
  GLuint normal_bo;
  GLuint meshlet_descriptor_ssbo;
  GLuint index_ssbo;
  GLuint visible_index_ssbo;
  GLuint draw_indirect_ssbo;
};

inline void MeshletTriangleMeshCsObjects::drawImGui()
{
  if (ImGui::TreeNode("Meshlet Triangle Mesh CS"))
  {
    ImGui::BulletText("MeshletTriangleMeshCSObjects");
    ImGui::TreePop();
  }
}

}  // namespace nell::comp
