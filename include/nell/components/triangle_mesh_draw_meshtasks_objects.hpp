#pragma once
#include <glad/glad.h>

#include <nell/components/buffer_objects.hpp>
#include <nell/components/ui_drawable.hpp>

namespace nell::comp
{
struct TriangleMeshDrawMeshtasksObjects final : BufferObjects, UiDrawable
{
  void drawImGui() override;
  GLuint vertex_ssbo;
  GLuint normal_ssbo;
  GLuint index_ssbo;

  GLuint limit_mesh_task_count = 65535;
};

inline void TriangleMeshDrawMeshtasksObjects::drawImGui()
{
  const ImU16 u16_min = 0, u16_max = 65535;
  const float drag_speed = 8.0f;
  ImGui::DragScalar("Mesh Task Limit", ImGuiDataType_U16,
                    &limit_mesh_task_count, drag_speed, &u16_min, &u16_max,
                    "%u");
  ImGui::BulletText("TriangleMeshDrawMeshtasksObjects");
}

}  // namespace nell::comp
