#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "components.hpp"

enum class GameState { IN_PROGRESS, LOSE, WIN };

enum class InputKind { UP, DOWN, LEFT, RIGHT };

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

  ecs::entities::EntityId add_render_info(ecs::Context<Registry> &ctx,
                                          components::RenderInfo &&render_info);
};
