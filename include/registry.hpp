#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <queue>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include "components.hpp"

enum class GameState { IN_PROGRESS, LOSE, WIN };

enum class InputKind { UP, DOWN, LEFT, RIGHT };

enum class TileType { ROAD, GRASS };

struct Registry {
  std::unordered_map<ecs::entities::EntityId, components::RenderInfo>
      render_infos;
  std::unordered_map<ecs::entities::EntityId, components::Character> characters;
  std::unordered_map<ecs::entities::EntityId, components::ActionRestriction>
      action_restrictions;
  std::unordered_map<ecs::entities::EntityId, components::Car> cars;
  GameState state = GameState::IN_PROGRESS;
  std::unordered_map<ecs::entities::EntityId, components::WinZone> win_zones;
  std::unordered_map<ecs::entities::EntityId, components::Animation> animations;
  std::unordered_map<ecs::entities::EntityId, components::ShoeItem> shoe_items;
  std::unordered_set<ecs::entities::EntityId> wheels;
  std::unordered_set<ecs::entities::EntityId> truck_plates;

  ecs::entities::EntityId character_id;
  std::queue<InputKind> input_queue;
  std::unordered_set<components::ActionKind> blocked_actions;
  bool pass_through;

  size_t map_bottom = 0;
  size_t map_top_generated = 0;
  std::uniform_int_distribution<int> random_tile_type_dist =
      std::uniform_int_distribution<int>(0, 1);
  std::uniform_int_distribution<int> random_tile_length_dist =
      std::uniform_int_distribution<int>(1, 3);

  ecs::entities::EntityId add_render_info(ecs::Context<Registry> &ctx,
                                          components::RenderInfo &&render_info);
  TileType random_tile_type(ecs::Context<Registry> &ctx);
  int random_tile_length(ecs::Context<Registry> &ctx);
};
