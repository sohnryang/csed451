#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include "registry.hpp"

namespace systems {
class Render : public ecs::systems::System<Registry> {
private:
  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override;

  void pre_update(ecs::Context<Registry> &ctx) override;

  void post_update(ecs::Context<Registry> &ctx) override;

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override;

public:
  Render();
};
} // namespace systems
