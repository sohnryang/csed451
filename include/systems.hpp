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

  void set_uniform_float(ecs::Context<Registry> &ctx, const char *name,
                         float value);

  void set_uniform_vec3(ecs::Context<Registry> &ctx, const char *name,
                        const glm::vec3 &value);

  void set_uniform_mat4(ecs::Context<Registry> &ctx, const char *name,
                        const glm::mat4 &value);

  void set_light_pos(ecs::Context<Registry> &ctx, const glm::vec3 &pos);

  void set_directional_light(ecs::Context<Registry> &ctx,
                             const glm::vec3 &direction);

  void set_ambient_intensity(ecs::Context<Registry> &ctx, float intensity);

  void set_diffuse_intensity_point(ecs::Context<Registry> &ctx,
                                   float intensity);

  void set_specular_intensity_point(ecs::Context<Registry> &ctx,
                                    float intensity);

  void set_diffuse_intensity_directional(ecs::Context<Registry> &ctx,
                                         float intensity);

  void set_specular_intensity_directional(ecs::Context<Registry> &ctx,
                                          float intensity);

  void set_projection_mat(ecs::Context<Registry> &ctx, const glm::mat4 &mat);

  void set_modelview_mat(ecs::Context<Registry> &ctx, const glm::mat4 &mat);

  void render_children(ecs::Context<Registry> &ctx, ecs::entities::EntityId id,
                       const glm::mat4 &base_mat);
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
