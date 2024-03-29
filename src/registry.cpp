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

TileType Registry::random_tile_type(ecs::Context<Registry> &ctx) {
  return static_cast<TileType>(random_tile_length_dist(ctx.random_gen()));
}

int Registry::random_tile_length(ecs::Context<Registry>& ctx) {
  return random_tile_length_dist(ctx.random_gen());
}

int Registry::random_column(ecs::Context<Registry> &ctx) {
  return random_column_dist(ctx.random_gen());
}