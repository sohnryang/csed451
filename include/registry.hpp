#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <unordered_map>

#include "components.hpp"

enum class GameState { IN_PROGRESS, LOSE, WIN };

struct Registry {
  std::unordered_map<ecs::entities::EntityId, components::RenderInfo>
      render_infos;
  std::unordered_map<ecs::entities::EntityId, components::Character> characters;
  std::unordered_map<ecs::entities::EntityId, components::ActionRestriction>
      action_restrictions;
  std::unordered_map<ecs::entities::EntityId, components::Car> cars;
  GameState state = GameState::IN_PROGRESS;
  std::unordered_map<ecs::entities::EntityId, components::WinZone> win_zones;
  ecs::entities::EntityId character_id;

  ecs::entities::EntityId add_render_info(ecs::Context<Registry> &ctx,
                                          components::RenderInfo &&render_info);
};
