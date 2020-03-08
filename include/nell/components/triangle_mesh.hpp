#pragma once
#include <assimp/postprocess.h>
#include <buffer_objects.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <mesh.hpp>
#include <ui_drawable.hpp>
#include <vector>

namespace nell::comp
{
struct TriangleMesh final : Mesh<TriangleMesh>, UiDrawable
{
  explicit TriangleMesh(aiMesh* ai_mesh);

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector <unsigned int> indices;

  size_t getVerticesSize() const { return vertices.size() * getVertexSizeT(); }
  size_t getNormalsSize() const { return normals.size() * getNormalSizeT(); }
  size_t getIndicesSize() const { return indices.size() * getIndexSizeT(); }

  static size_t getVertexSizeT() { return 3 * sizeof(GLfloat); }
  static size_t getNormalSizeT() { return 3 * sizeof(GLfloat); }
  static size_t getIndexSizeT() { return sizeof(GLuint); }

  static GLint getVertexSize() { return 3; }
  static GLint getNormalSize() { return 3; }

  static GLenum getVertexType() { return GL_FLOAT; }
  static GLenum getNormalType() { return GL_FLOAT; }

  void drawImGui() override;
  static unsigned getImportFlags () { return aiProcess_GenSmoothNormals;}
};



}  // namespace nell::comp
