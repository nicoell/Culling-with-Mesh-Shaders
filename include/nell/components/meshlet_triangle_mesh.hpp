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
struct VertexDescriptor
{
  alignas(16) glm::vec3 position;
  alignas(16) glm::vec3 normal;
};

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
      std::vector<VertexDescriptor *> &vertex_descriptors);
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
      std::vector<gpu::VertexDescriptor *> &vertex_descriptors);
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

  std::vector<gpu::VertexDescriptor> vertex_descriptors;
  std::vector<gpu::MeshletDescriptor> meshlet_descriptors;
  std::vector<GLuint> indices;

  bool cull_front_or_back;
  float debug_value = 0.0f;

  size_t getVertexDescriptorSize() const
  {
    return vertex_descriptors.size() * sizeof(gpu::VertexDescriptor);
  }
  size_t getMeshletDescriptorSize() const
  {
    return meshlet_descriptors.size() * sizeof(gpu::MeshletDescriptor);
  }
  size_t getIndicesSize() const { return indices.size() * sizeof(GLuint); }

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
