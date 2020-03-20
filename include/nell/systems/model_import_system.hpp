#pragma once
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <entt/entt.hpp>
#include <nell/components/child_parent_relationship.hpp>
#include <nell/components/entity_name.hpp>
#include <nell/components/model_source.hpp>
#include <nell/components/transform.hpp>
#include <nell/definitions.hpp>
#include <queue>

namespace nell::systems
{
template <typename... MeshComponent>
void processNode(entt::registry &reg, entt::entity &parent_entity,
                 entt::entity &current_entity, const aiScene *scene,
                 aiNode *node)
{
  // Name
  auto node_name = std::string(node->mName.C_Str());
  node_name += ":Node";
  reg.assign<comp::EntityName>(current_entity, node_name);

  // Parent Link
  auto &parent_link = reg.assign<comp::ParentLink>(current_entity);
  parent_link.parent = parent_entity;

  // Transform
  {
    auto &tf = reg.assign<comp::Transform>(current_entity);

    aiVector3t<float> ai_scaling;
    aiQuaterniont<float> ai_rotation;
    aiVector3t<float> ai_position;

    node->mTransformation.Decompose(ai_scaling, ai_rotation, ai_position);
    const auto scaling = glm::vec3(ai_scaling.x, ai_scaling.y, ai_scaling.z);
    const auto quat =
        glm::quat(ai_rotation.w, ai_rotation.x, ai_rotation.y, ai_rotation.z);
    const auto translation =
        glm::vec3(ai_position.x, ai_position.y, ai_position.z);

    tf.setScale(scaling);
    tf.setTranslation(translation);
    tf.setRotation(quat);
  }
  if (node->mNumMeshes + node->mNumChildren > 0)
  {
    std::vector<entt::entity> children;
    children.reserve(node->mNumMeshes + node->mNumChildren);

    for (unsigned int i_mesh = 0; i_mesh < node->mNumMeshes; i_mesh++)
    {
      auto mesh_entity_name = node_name;
      mesh_entity_name += ":";
      mesh_entity_name += std::to_string(i_mesh);

      auto mesh_entity = reg.create();
      reg.assign<comp::EntityName>(mesh_entity, mesh_entity_name);
      reg.assign<comp::Transform>(mesh_entity);
      auto &mesh_parent_link = reg.assign<comp::ParentLink>(mesh_entity);
      mesh_parent_link.parent = current_entity;

      children.push_back(mesh_entity);

      aiMesh *ai_mesh = scene->mMeshes[node->mMeshes[i_mesh]];

      (reg.assign<MeshComponent>(mesh_entity, ai_mesh), ...);
    }

    for (unsigned int k = 0; k < node->mNumChildren; k++)
    {
      auto child_entity = reg.create();
      children.push_back(child_entity);

      processNode<MeshComponent...>(reg, current_entity, child_entity, scene,
                                    node->mChildren[k]);
    }

    auto &child_link = reg.assign<comp::ChildLink>(current_entity);
    child_link.children = children;
  }
}

template <typename... MeshComponent>
void importModelsFromSource(entt::registry &reg)
{
  auto model_source_view = reg.view<comp::EntityName, comp::ModelSource>();

  Assimp::Importer importer;

  for (auto entity : model_source_view)
  {
    auto [entity_name, asset_source_comp] =
        model_source_view.get<comp::EntityName, comp::ModelSource>(entity);

    auto parent_name = entity_name.name;

    unsigned combined_flags = 0;
    // combined_flags |= (requestImportFlags<MeshComponent>(), ...);
    combined_flags = (MeshComponent::requestImportFlags() | ...);

    const aiScene *scene =
        importer.ReadFile(kAssetPath + asset_source_comp.path, combined_flags);

    if (!scene)
    {
      spdlog::warn("Unable to load Asset {}{}", kAssetPath,
                   asset_source_comp.path);
      continue;
    }

    auto &child_link = reg.assign<comp::ChildLink>(entity);

    auto child_entity = reg.create();
    child_link.children.push_back(child_entity);

    processNode<MeshComponent...>(reg, entity, child_entity, scene,
                                  scene->mRootNode);
  }
}

}  // namespace nell::systems
