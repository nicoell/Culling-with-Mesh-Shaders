#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <nell/components/model_source.hpp>
#include <vector>

namespace nell::comp
{
struct Model
{
  // TODO: Seperate this as base class, allowing different mesh definitions. Or
  // allow the activation of specific stuff. Make this also work together with
  // the importer.
  struct Mesh
  {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    size_t getVerticesSize() const
    {
      return vertices.size() * getVertexSizeT();
    }
    size_t getNormalsSize() const { return normals.size() * getNormalSizeT(); }
    size_t getIndicesSize() const { return indices.size() * getIndexSizeT(); }

    size_t getVertexSizeT() const { return 3 * sizeof(GLfloat); }
    size_t getNormalSizeT() const { return 3 * sizeof(GLfloat); }
    size_t getIndexSizeT() const { return sizeof(GLuint); }

    GLint getVertexSize() const { return 3; }
    GLint getNormalSize() const { return 3; }

    GLenum getVertexType() const { return GL_FLOAT; }
    GLenum getNormalType() const { return GL_FLOAT; }
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

}  // namespace nell::comp
