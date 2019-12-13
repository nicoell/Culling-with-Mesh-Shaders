#include <nell/components/mesh.hpp>
#include <nell/definitions.hpp>
#include <nell/systems/model_import_system.hpp>

namespace nell::systems
{
void importAssets(entt::registry &reg)
{
  auto asset_source_view = reg.view<comp::ModelSource>();

  Assimp::Importer importer;

  for (auto entity : asset_source_view)
  {
    auto &asset_source_comp = asset_source_view.get(entity);

    const aiScene *scene =
        importer.ReadFile(kAssetPath + asset_source_comp.path, 0);

    if (!scene)
    {
      spdlog::warn("Unable to load Asset {}{}", kAssetPath,
                   asset_source_comp.path);
      continue;
    }

    // TODO eventually handle mats, textures, etc.

    auto &model_comp = reg.assign<comp::Model>(entity);

    for (unsigned int i_mesh = 0; i_mesh < scene->mNumMeshes; i_mesh++)
    {
      aiMesh *ai_mesh = scene->mMeshes[i_mesh];
      comp::Model::Mesh mesh;

      for (unsigned int i_vert = 0; i_vert < ai_mesh->mNumVertices; i_vert++)
      {
        aiVector3D ai_vec = ai_mesh->mVertices[i_vert];
        mesh.vertices.emplace_back(ai_vec.x, ai_vec.y, ai_vec.z);

        if (ai_mesh->HasNormals())
        {
          ai_vec = ai_mesh->mNormals[i_vert];
          mesh.normals.emplace_back(ai_vec.x, ai_vec.y, ai_vec.z);
        }
      }

      for (unsigned int i_face = 0; i_face < ai_mesh->mNumFaces; ++i_face)
      {
        aiFace ai_face = ai_mesh->mFaces[i_face];
        for (unsigned int i_indx = 0; i_indx < ai_face.mNumIndices; ++i_indx)
        {
          mesh.indices.push_back(ai_face.mIndices[i_indx]);
        }
      }

      model_comp.mesh_list.push_back(mesh);
    }

    // load model and attach mesh as component to entity
  }
}
}  // namespace nell