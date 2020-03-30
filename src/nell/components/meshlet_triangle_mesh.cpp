#include <spdlog/spdlog.h>

#include <glm/gtc/constants.hpp>
#include <map>
#include <nell/components/meshlet_triangle_mesh.hpp>

namespace nell::gpu
{
BoundingCone BoundingCone::GetApproximateReflexBoundingCone(
    std::vector<VertexDescriptor *> &vertex_descriptors)
{
  glm::vec3 mean_normal{};
  for (auto vertex_descriptor : vertex_descriptors)
  {
    mean_normal += vertex_descriptor->normal;
  }
  // mean_normal /= vertex_descriptors.size();
  mean_normal = glm::normalize(mean_normal);

  auto length = glm::length((mean_normal));
  /*
  float max_distance = 0.0f;
  for (auto vertex_descriptor : vertex_descriptors)
  {
    max_distance = glm::max(
        max_distance, glm::distance(mean_normal, vertex_descriptor->normal));
  }

  // Convert euclidean distance to spherical distance
  // This is equal to acos(dot(mean_normal, max_distance_vector)) for unit
  // vectors
  float angular_span = 2.0f * glm::asin(0.5f * max_distance);*/

  float angular_span = 0.0f;
  for (auto vertex_descriptor : vertex_descriptors)
  {
    angular_span =
        glm::max(angular_span,
                 glm::acos(glm::dot(mean_normal, vertex_descriptor->normal)));
  }

  // Test correctness
  /*
  float mean_dotp = 0;
  for (auto vertex_descriptor : vertex_descriptors)
  {
    float dotp = glm::dot(mean_normal, vertex_descriptor->normal);
    float testcos = glm::cos(glm::min(angular_span, glm::half_pi<float>()));
    mean_dotp += dotp;
    if (testcos - dotp > 0.01f)
    {
      spdlog::error("Vertex Normal is outside bounding cone");
    }
  }
  mean_dotp /= vertex_descriptors.size();*/

  BoundingCone cone{.normal = mean_normal, .angle = angular_span};
  return cone;
}
}  // namespace nell::gpu

namespace nell::comp
{
AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    std::vector<gpu::VertexDescriptor *> &vertex_descriptors)
{
  _min = vertex_descriptors[0]->position;
  _max = _min;
  for (auto vertex_descriptor : vertex_descriptors)
  {
    const auto position = vertex_descriptor->position;
    _min = glm::min(_min, position);
    _max = glm::max(_max, position);
  }
  _extents = (_max - _min) / 2.f;
  _center = _min + _extents;
}

MeshletTriangleMesh::MeshletTriangleMesh(aiMesh *ai_mesh)
{
  const auto vertex_count = ai_mesh->mNumVertices;
  vertex_descriptors.reserve(vertex_count);
  for (unsigned i_vert = 0; i_vert < vertex_count; i_vert++)
  {
    aiVector3D ai_pos = ai_mesh->mVertices[i_vert];
    aiVector3D ai_normal = ai_mesh->mNormals[i_vert];

    vertex_descriptors.push_back(gpu::VertexDescriptor{
        .position = glm::vec3(ai_pos.x, ai_pos.y, ai_pos.z),
        .normal = glm::vec3(ai_normal.x, ai_normal.y, ai_normal.z)});
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

  std::map<unsigned, gpu::VertexDescriptor *> meshlet_vertex_descriptor_map;
  std::vector<gpu::VertexDescriptor *> meshlet_vertex_descriptors;

  for (unsigned i_meshlet = 0; i_meshlet < meshlet_count; ++i_meshlet)
  {
    const auto i_face_to =
        glm::min(i_face_from + _meshlet_primitive_count, face_count);
    GLushort primitive_count = i_face_to - i_face_from;
    meshlet_vertex_descriptor_map.clear();
    meshlet_vertex_descriptors.clear();
    meshlet_vertex_descriptors.reserve(max_indices_count);

    // Iterate primitives of this meshlet
    for (unsigned i_face = i_face_from; i_face < i_face_to; ++i_face)
    {
      const aiFace ai_face = ai_mesh->mFaces[i_face];
      for (unsigned i_indx = 0; i_indx < ai_face.mNumIndices; ++i_indx)
      {
        unsigned index = ai_face.mIndices[i_indx];
        indices.push_back(index);

        gpu::VertexDescriptor *vertex_descriptor =
            &vertex_descriptors.at(index);

        // Build vertex descriptor map with unique vertices
        if (meshlet_vertex_descriptor_map
                .insert(std::make_pair(index, vertex_descriptor))
                .second)
        {
          // And vector, too
          meshlet_vertex_descriptors.push_back(vertex_descriptor);
        }
      }
    }

    auto aabb = AxisAlignedBoundingBox(meshlet_vertex_descriptors);
    gpu::BoundingSphere bounding_sphere{
        .center = aabb.getCenter(), .radius = [&]() -> float {
          float radius = 0.f;
          for (auto vertex_descriptor : meshlet_vertex_descriptors)
          {
            radius =
                glm::max(radius, glm::distance(bounding_sphere.center,
                                               vertex_descriptor->position));
          }
          return radius;
        }()};
    gpu::BoundingCone bounding_cone =
        gpu::BoundingCone::GetApproximateReflexBoundingCone(
            meshlet_vertex_descriptors);

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
    ImGui::Text("%s: %d", "vertex_descriptors", vertex_descriptors.size());
    ImGui::Text("%s: %d", "meshlet_descriptors", meshlet_descriptors.size());
    ImGui::Text("%s: %d", "Indices", indices.size());
    ImGui::TreePop();
  }
}

}  // namespace nell::comp
