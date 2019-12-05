#pragma once
#include <glm/glm.hpp>
#include <nell/components/asset_source_path.hpp>
#include <vector>

namespace nell::comp
{
struct Model
{
  struct Mesh
  {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
  };

  std::vector<Mesh> mesh_list;
};

inline void drawComponentImpl(const Model &model)
{
  if (ImGui::TreeNode("Mesh Component"))
  {
    for (unsigned int i = 0; i < model.mesh_list.size(); ++i)
    {
      if (ImGui::TreeNode((void *)(intptr_t)i, "Mesh %d", i))
      {
        ImGui::Text("%s: %d", "Vertices", model.mesh_list[i].vertices.size());
        ImGui::Text("%s: %d", "Normals", model.mesh_list[i].normals.size());
        ImGui::Text("%s: %d", "Indices", model.mesh_list[i].indices.size());
        ImGui::TreePop();
      }
    }

    ImGui::TreePop();
  }
}

}  // namespace nell
