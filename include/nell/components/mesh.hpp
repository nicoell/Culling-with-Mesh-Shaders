#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui/misc/imgui_stdlib.h>
#include <vector>

namespace nell::comp
{
struct Mesh
{
  virtual ~Mesh () = default;
  virtual void drawImGui() const = 0;
};

template <class T>
struct Model
{
  static_assert(std::is_base_of<Mesh, T>::value, "T must be a Mesh");
  std::vector<T> mesh_list;
};

template<class T>
inline void drawComponentImpl(const Model<T> &model)
{
  if (ImGui::TreeNode("Mesh Component"))
  {
    for (unsigned int i = 0; i < model.mesh_list.size(); ++i)
    {
      if (ImGui::TreeNode((void *)(intptr_t)i, "Mesh %d", i))
      {
        model.mesh_list[i].drawImGui();
        ImGui::TreePop();
      }
    }

    ImGui::TreePop();
  }
}

}  // namespace nell::comp
