#include <spdlog/spdlog.h>

#include <glm/gtc/constants.hpp>
#include <map>
#include <nell/components/meshlet_triangle_mesh.hpp>

namespace nell::gpu
{
BoundingCone BoundingCone::GetApproximateReflexBoundingCone(
    std::vector<glm::vec4 *> &normals)
{
  glm::vec3 mean_normal{};
  for (auto normal : normals)
  {
    mean_normal += glm::vec3(*normal);
  }
  mean_normal = glm::normalize(mean_normal);

  float angular_span = 0.0f;
  for (auto normal : normals)
  {
    angular_span =
        glm::max(angular_span,
                 glm::acos(glm::dot(mean_normal, glm::vec3(*normal))));
  }

  BoundingCone cone{.normal = mean_normal, .angle = angular_span};
  return cone;
}
}  // namespace nell::gpu

namespace nell::comp
{
AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    std::vector<glm::vec4 *> &vertices)
{
  _min = *vertices[0];
  _max = _min;
  for (auto vertex : vertices)
  {
    _min = glm::min(_min, glm::vec3(*vertex));
    _max = glm::max(_max, glm::vec3(*vertex));
  }
  _extents = (_max - _min) / 2.f;
  _center = _min + _extents;
}

MeshletTriangleMesh::MeshletTriangleMesh(aiMesh *ai_mesh)
{
  const auto vertex_count = ai_mesh->mNumVertices;
  vertices.reserve(vertex_count);
  normals.reserve(vertex_count);
  for (unsigned i_vert = 0; i_vert < vertex_count; i_vert++)
  {
    aiVector3D ai_pos = ai_mesh->mVertices[i_vert];
    aiVector3D ai_normal = ai_mesh->mNormals[i_vert];

    vertices.emplace_back(ai_pos.x, ai_pos.y, ai_pos.z, 1);
    normals.emplace_back(ai_normal.x, ai_normal.y, ai_normal.z, 0);
  }

  // Iterate Meshlets

  const auto max_indices_count = ai_mesh->mNumFaces * 3;
  indices.reserve(max_indices_count);
  const auto meshlet_count =
      ai_mesh->mNumFaces / _meshlet_primitive_count +
      (ai_mesh->mNumFaces % _meshlet_primitive_count != 0);
  meshlet_descriptors.reserve(meshlet_count);  // Round up integer division

  auto i_face_from = 0u;
  const auto face_count = ai_mesh->mNumFaces;

  std::map<unsigned, bool> unique_index_map;
  std::vector<glm::vec4 *> meshlet_vertices;
  std::vector<glm::vec4 *> meshlet_normals;

  for (unsigned i_meshlet = 0; i_meshlet < meshlet_count; ++i_meshlet)
  {
    const auto i_face_to =
        glm::min(i_face_from + _meshlet_primitive_count, face_count);
    GLushort primitive_count = i_face_to - i_face_from;
    unique_index_map.clear();

    meshlet_vertices.clear();
    meshlet_vertices.reserve(max_indices_count);

    meshlet_normals.clear();
    meshlet_normals.reserve(max_indices_count);

    // Iterate primitives of this meshlet
    for (unsigned i_face = i_face_from; i_face < i_face_to; ++i_face)
    {
      const aiFace ai_face = ai_mesh->mFaces[i_face];
      for (unsigned i_indx = 0; i_indx < ai_face.mNumIndices; ++i_indx)
      {
        unsigned index = ai_face.mIndices[i_indx];
        indices.push_back(index);

        // Add to map to test if index already present
        if (unique_index_map
                .insert(std::make_pair(index, true))
                .second)
        {
          auto *vertex = &vertices.at(index);
          auto *normal = &normals.at(index);

          meshlet_vertices.push_back(vertex);
          meshlet_normals.push_back(normal);
        }
      }
    }

    auto aabb = AxisAlignedBoundingBox(meshlet_vertices);
    gpu::BoundingSphere bounding_sphere{
        .center = aabb.getCenter(), .radius = [&]() -> float {
          float radius = 0.f;
          for (auto vertex : meshlet_vertices)
          {
            radius =
                glm::max(radius, glm::distance(bounding_sphere.center,
                                               glm::vec3(*vertex)));
          }
          return radius;
        }()};
    gpu::BoundingCone bounding_cone =
        gpu::BoundingCone::GetApproximateReflexBoundingCone(
            meshlet_normals);

    meshlet_descriptors.push_back(
        gpu::MeshletDescriptor{.sphere = bounding_sphere,
                               .cone = bounding_cone,
                               .primitive_count = primitive_count});

    i_face_from += _meshlet_primitive_count;
  }
}

void MeshletTriangleMesh::drawImGui()
{
  if (ImGui::TreeNode("Meshlet Triangle Mesh"))
  {
    ImGui::Text("%s: %d", "meshlet_descriptors", meshlet_descriptors.size());

    ImGui::Text("%s: %d", "vertices", vertices.size());
    ImGui::Text("%s: %d", "normals", normals.size());
    ImGui::Text("%s: %d", "Indices", indices.size());
    ImGui::TreePop();
  }
}

}  // namespace nell::comp
