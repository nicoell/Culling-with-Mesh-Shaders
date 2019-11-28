#include <asset_import_system.hpp>

namespace nell
{
void importAssets(entt::registry &reg)
{
  auto model_load_view = reg.view<AssetSourcePath>();

  for (auto entity : model_load_view)
  {
    auto &model_load_component = model_load_view.get(entity);
    // load model and attach mesh as component to entity
  }
}
}  // namespace nell