#pragma once
#include <entt/entt.hpp>
#include <nell/components/asset_source_path.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace nell::systems
{
void importAssets(entt::registry &);
}