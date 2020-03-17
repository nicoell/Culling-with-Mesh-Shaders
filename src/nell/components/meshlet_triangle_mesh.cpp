#include <nell/components/meshlet_triangle_mesh.hpp>

namespace nell::comp
{
MeshletTriangleMesh::MeshletTriangleMesh(aiMesh *ai_mesh)
{
  vertices.reserve(ai_mesh->mNumVertices);
  if (ai_mesh->HasNormals()) normals.reserve(ai_mesh->mNumVertices);
  indices.reserve(ai_mesh->mNumFaces * 3);

  for (unsigned int i_vert = 0; i_vert < ai_mesh->mNumVertices; i_vert++)
  {
    aiVector3D ai_vec = ai_mesh->mVertices[i_vert];
    vertices.emplace_back(ai_vec.x, ai_vec.y, ai_vec.z);

    if (ai_mesh->HasNormals())
    {
      ai_vec = ai_mesh->mNormals[i_vert];
      normals.emplace_back(ai_vec.x, ai_vec.y, ai_vec.z);
    }
  }

  for (unsigned int i_face = 0; i_face < ai_mesh->mNumFaces; ++i_face)
  {
    aiFace ai_face = ai_mesh->mFaces[i_face];
    for (unsigned int i_indx = 0; i_indx < ai_face.mNumIndices; ++i_indx)
    {
      indices.push_back(ai_face.mIndices[i_indx]);
    }
  }
}

void MeshletTriangleMesh::drawImGui()
{
  ImGui::Text("%s: %d", "Vertices", vertices.size());
  ImGui::Text("%s: %d", "Normals", normals.size());
  ImGui::Text("%s: %d", "Indices", indices.size());
}

}  // namespace nell::comp
