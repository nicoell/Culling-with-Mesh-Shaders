#pragma once
#include <callbacks/render_callback.hpp>
#include <callbacks/update_callback.hpp>
#include <entt/entity/registry.hpp>

namespace nell::systems
{
class BasicShadingSystem final : public UpdateCallback, public RenderCallback
{
public:
  BasicShadingSystem();
  ~BasicShadingSystem () override;
  void update(const double &time, const double &delta_time, entt::registry &reg) override;
  void render(const double &time, const double &delta_time, entt::registry &reg) override;
};

}  // namespace nell::systems