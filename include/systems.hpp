#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <glm/glm.hpp>

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

  void render_single(ecs::Context<Registry> &ctx, const components::Mesh &mesh);

  void render_children(ecs::Context<Registry> &ctx, ecs::entities::EntityId id);

public:
  Render();
};

class InputHandler : public ecs::systems::System<Registry> {
private:
  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override;

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override;
};

class Character : public ecs::systems::System<Registry> {
private:
  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override;

  void pre_update(ecs::Context<Registry> &ctx) override;

  void post_update(ecs::Context<Registry> &ctx) override;

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override;
};

class Car : public ecs::systems::System<Registry> {
private:
  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override;

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override;
};

class Animation : public ecs::systems::System<Registry> {
private:
  glm::mat4 interpolate_transforms(float ratio, const glm::mat4 &first,
                                   const glm::mat4 &second);

  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override;

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override;

public:
  static void set(ecs::Context<Registry> &ctx, ecs::entities::EntityId id,
                  components::AnimationInfo &&animation_info);

  static void reset(ecs::Context<Registry> &ctx, ecs::entities::EntityId id);

  static void disable(ecs::Context<Registry> &ctx, ecs::entities::EntityId id);
};
} // namespace systems
