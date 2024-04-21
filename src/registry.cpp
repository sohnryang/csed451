#include <registry.hpp>

#include <ecs/entities.hpp>
#include <ecs/systems.hpp>

#include <utility>

#include <components.hpp>

ecs::entities::EntityId Registry::add_mesh(ecs::Context<Registry> &ctx,
                                           components::Mesh &&mesh) {
  auto id = ctx.entity_manager().next_id();
  meshes[id] = std::move(mesh);
  return id;
}

TileType Registry::random_tile_type(ecs::Context<Registry> &ctx) {
  return static_cast<TileType>(random_tile_type_dist(ctx.random_gen()));
}

int Registry::random_tile_length(ecs::Context<Registry> &ctx) {
  return random_tile_length_dist(ctx.random_gen());
}

int Registry::random_column(ecs::Context<Registry> &ctx) {
  return random_column_dist(ctx.random_gen());
}

int Registry::random_column(ecs::Context<Registry> &ctx,
                            std::vector<bool> &ref) {
  while (true) {
    int ret = random_column_dist(ctx.random_gen());
    if (!ref[ret])
      return ret;
  }
}

int Registry::random_tree_number(ecs::Context<Registry> &ctx) {
  return random_tree_number_dist(ctx.random_gen());
}

double Registry::random_speed(ecs::Context<Registry> &ctx) {
  while (true) {
    double ret = random_speed_dist(ctx.random_gen());
    if (std::abs(ret) < 0.5)
      continue;
    return ret;
  }
}

bool Registry::random_probability(ecs::Context<Registry> &ctx, double p) {
  return random_probability_dist(ctx.random_gen()) <= p;
}
