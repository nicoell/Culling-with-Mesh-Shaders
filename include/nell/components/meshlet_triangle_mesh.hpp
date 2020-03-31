#pragma once
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <nell/components/mesh.hpp>
#include <nell/components/ui_drawable.hpp>
#include <vector>

namespace nell::gpu
{

struct BoundingSphere
{
  alignas(16) glm::vec3 center;
  alignas(4) float radius;
};

struct BoundingCone
{
  alignas(16) glm::vec3 normal;
  alignas(4) float angle;

  static BoundingCone GetApproximateReflexBoundingCone(
      std::vector<glm::vec4 *> &normals);
};

struct MeshletDescriptor
{
  BoundingSphere sphere;
  BoundingCone cone;
  GLushort primitive_count;
};
}  // namespace nell::gpu

namespace nell::comp
{
class AxisAlignedBoundingBox
{
 public:
  explicit AxisAlignedBoundingBox(
      std::vector<glm::vec4 *> &vertices);
  const glm::vec3 &getCenter() const { return _center; };
  const glm::vec3 &getExtents() const { return _extents; };
  const glm::vec3 &getMin() const { return _min; };
  const glm::vec3 &getMax() const { return _max; };

 private:
  glm::vec3 _center{};
  glm::vec3 _extents{};

  glm::vec3 _min{};
  glm::vec3 _max{};
};

struct MeshletTriangleMesh final : Mesh<MeshletTriangleMesh>, UiDrawable
{
  explicit MeshletTriangleMesh(aiMesh *ai_mesh);

  std::vector<gpu::MeshletDescriptor> meshlet_descriptors;
  std::vector<glm::vec4> vertices;
  std::vector<glm::vec4> normals;
  std::vector<GLuint> indices;

  size_t getVerticesSize() const { return vertices.size() * sizeof(glm::vec4); }
  size_t getNormalsSize() const { return normals.size() * sizeof(glm::vec4); }
  size_t getMeshletDescriptorSize() const
  {
    return meshlet_descriptors.size() * sizeof(gpu::MeshletDescriptor);
  }
  size_t getIndicesSize() const { return indices.size() * sizeof(GLuint); }

  static size_t getVertexSizeT() { return sizeof(glm::vec4); }
  static size_t getNormalSizeT() { return sizeof(glm::vec4); }
  static size_t getIndexSizeT() { return sizeof(GLuint); }
  
  static GLint getVertexSize() { return 4; }
  static GLint getNormalSize() { return 4; }

  static GLenum getVertexType() { return GL_FLOAT; }
  static GLenum getNormalType() { return GL_FLOAT; }

  void drawImGui() override;
  static unsigned getImportFlags()
  {
    return (aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality | aiProcess_Triangulate);
  }

 private:
  GLushort _meshlet_primitive_count = 32;
};

}  // namespace nell::comp
