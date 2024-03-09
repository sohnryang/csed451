#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <unordered_map>

#include "components.hpp"

struct Registry {
  std::unordered_map<ecs::entities::EntityId, components::RenderInfo>
      render_infos;
  std::unordered_map<ecs::entities::EntityId, components::Transform> transforms;
  std::unordered_map<ecs::entities::EntityId, components::Character> characters;
  std::unordered_map<ecs::entities::EntityId, components::ActionRestriction>
      action_restrictions;
  std::unordered_map<ecs::entities::EntityId, components::Car> cars;

  ecs::entities::EntityId add_render_info(ecs::Context<Registry> &ctx,
                                          components::RenderInfo &&render_info);
};
