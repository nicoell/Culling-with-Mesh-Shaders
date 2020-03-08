#include <components/basic_shading_objects.hpp>

namespace nell::comp
{
void BasicShadingMesh::drawImGui() const
{
  ImGui::Text("%s: %d", "Vertices", vertices.size());
  ImGui::Text("%s: %d", "Normals", normals.size());
  ImGui::Text("%s: %d", "Indices", indices.size());
}

}  // namespace nell::comp
