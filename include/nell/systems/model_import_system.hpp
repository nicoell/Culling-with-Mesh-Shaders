#pragma once
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <entity_name.hpp>
#include <entt/entt.hpp>

#include <nell/definitions.hpp>
#include <nell/components/model_source.hpp>
#include <transform.hpp>

namespace nell::systems
{
//void importAssets(entt::registry &);

/*template <typename MeshComponent>
void processMesh(entt::registry &reg, entt::entity &entity, aiMesh *ai_mesh)
{
  auto &mesh_comp = reg.assign<MeshComponent>(entity, ai_mesh);
}

template <typename MeshComponent>
void initMeshComponent(entt::registry &reg, entt::entity &entity)
{
  reg.assign_or_replace<MeshComponent>(entity);
}

template <typename MeshComponent>
unsigned requestImportFlags()
{
  unsigned flags = MeshComponent::requestImportFlags();
  return flags;
}*/

template <typename... MeshComponent>
void importModelsFromSource(entt::registry &reg)
{
  auto model_source_view = reg.view<comp::EntityName, comp::ModelSource>();

  Assimp::Importer importer;

  for (auto entity : model_source_view)
  {
    auto [entity_name, asset_source_comp] = model_source_view.get <comp::EntityName,
         comp::ModelSource>(entity);

    auto parent_name = entity_name.name;

    unsigned combined_flags = 0;
    //combined_flags |= (requestImportFlags<MeshComponent>(), ...);
    combined_flags = (MeshComponent::requestImportFlags() || ...);

    const aiScene *scene =
        importer.ReadFile(kAssetPath + asset_source_comp.path, combined_flags);

    if (!scene)
    {
      spdlog::warn("Unable to load Asset {}{}", kAssetPath,
                   asset_source_comp.path);
      continue;
    }

    for (unsigned int i_mesh = 0; i_mesh < scene->mNumMeshes; i_mesh++)
    {
      auto child_entity = reg.create();
      reg.assign<comp::EntityName>(child_entity, parent_name + ":" + std::to_string(i_mesh));
      reg.assign<comp::Transform>(child_entity);

      //TODO: attach child to parent for transform

      aiMesh *ai_mesh = scene->mMeshes[i_mesh];
      (reg.assign<MeshComponent>(child_entity, ai_mesh), ...);

      //(initMeshComponent<MeshComponent>(reg, child_entity), ...);
      //(processMesh<MeshComponent>(reg, entity, ai_mesh), ...);
    }
  }
}

}  // namespace nell::systems
