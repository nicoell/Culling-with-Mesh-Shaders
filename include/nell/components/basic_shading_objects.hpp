#pragma once
#include <glad/glad.h>

#include <mesh.hpp>

namespace nell::comp
{
// TODO: Refactor this together with mesh struct.

struct BasicShadingObjects
{
  GLuint vertex_buffer_object;
  GLuint normal_buffer_object;
  GLuint index_buffer_object;
};

struct BasicShadingMesh final : Mesh
{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;

  BasicShadingObjects bs_objects;

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

  void drawImGui() const override;
};



}  // namespace nell::comp
