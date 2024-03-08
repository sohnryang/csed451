#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <queue>

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

enum class InputKind { UP, DOWN, LEFT, RIGHT };

class InputHandler : public ecs::systems::System<Registry> {
private:
  std::queue<InputKind> _input_queue;

  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override;

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override;

public:
  InputHandler();

  void push_input(InputKind input);
};
} // namespace systems
