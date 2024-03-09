#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <glm/glm.hpp>

#include <queue>
#include <unordered_set>

#include "components.hpp"
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

class Character : public ecs::systems::System<Registry> {
private:
  bool character_found;
  ecs::entities::EntityId character_id;
  std::unordered_set<components::ActionKind> blocked_actions;

  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override;

  void pre_update(ecs::Context<Registry> &ctx) override;

  void post_update(ecs::Context<Registry> &ctx) override;

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override;

public:
  static bool intersect(glm::vec2 top_left1, glm::vec2 bottom_right1,
                        glm::vec2 top_left2, glm::vec2 bottom_right2);

  Character();
};

class Car : public ecs::systems::System<Registry> {
private:
  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override;

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override;

public:
  Car();
};
} // namespace systems
