#include "registry.hpp"

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <utility>

#include "components.hpp"

ecs::entities::EntityId
Registry::add_render_info(ecs::Context<Registry> &ctx,
                          components::RenderInfo &&render_info) {
  auto id = ctx.entity_manager().next_id();
  render_infos[id] = std::move(render_info);
  return id;
}
