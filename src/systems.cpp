#include "systems.hpp"

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <cmath>
#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#include <cstddef>
#include <iterator>
#include <string>

#include "components.hpp"
#include "grid.hpp"
#include "registry.hpp"
#include "scene.hpp"

namespace systems {
bool Render::should_apply(ecs::Context<Registry> &ctx,
                          ecs::entities::EntityId id) {
  return ctx.registry().render_infos.count(id) &&
         ctx.entity_manager().entity_graph()[id].parent == id;
}

void Render::pre_update(ecs::Context<Registry> &ctx) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
}

void Render::post_update(ecs::Context<Registry> &ctx) {
  const auto state = ctx.registry().state;
  if (state != GameState::IN_PROGRESS) {
    const std::string text =
        state == GameState::LOSE ? "GAME OVER" : "YOU WIN!!";
    glColor3f(1, 1, 0);
    glLineWidth(5);

    glPushMatrix();
    const auto char_width = 0.2f;

    for (std::size_t i = 0; i < text.length(); i++) {
      glLoadIdentity();
      glTranslatef(-0.95f + i * char_width, 0.0, 0.75);
      glScalef(1.0f / 400, 1.0f / 400, 1);
      glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, text[i]);
    }
    glPopMatrix();
  }
  glutSwapBuffers();
}

void Render::update_single(ecs::Context<Registry> &ctx,
                           ecs::entities::EntityId id) {
  const auto &render_info = ctx.registry().render_infos.at(id);
  const auto &animations = ctx.registry().animations;
  glPushMatrix();
  if (animations.count(id))
    glLoadMatrixf(glm::value_ptr(render_info.mat * animations.at(id).mat));
  else
    glLoadMatrixf(glm::value_ptr(render_info.mat));
  render_single(render_info);
  render_children(ctx, id);
  glPopMatrix();
}

void Render::render_single(const components::RenderInfo &render_info) {
  const auto &color = render_info.color;
  glColor3f(color.r, color.g, color.b);
  glBegin(GL_POLYGON);
  const auto &vertices = render_info.vertex_container->vertices();
  for (const auto &v : vertices)
    glVertex3f(v[0], v[1], v[2]);
  glEnd();
}

void Render::render_children(ecs::Context<Registry> &ctx,
                             ecs::entities::EntityId id) {
  const auto &render_infos = ctx.registry().render_infos;
  const auto &animations = ctx.registry().animations;

  float matrix_raw[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix_raw);
  const auto base_mat = glm::make_mat4(matrix_raw);

  for (const auto child_id : ctx.entity_manager().entity_graph()[id].children) {
    if (!render_infos.count(child_id))
      continue;

    glPushMatrix();
    const auto &render_info = render_infos.at(child_id);
    const auto child_mat = base_mat * render_info.mat;
    if (animations.count(child_id))
      glLoadMatrixf(glm::value_ptr(child_mat * animations.at(child_id).mat));
    else
      glLoadMatrixf(glm::value_ptr(child_mat));
    render_single(render_info);
    render_children(ctx, child_id);
    glPopMatrix();
  }
}

Render::Render() {
  glClearColor(0, 0, 0, 1);
  glDepthFunc(GL_LESS);
  glDepthRange(0, 1);
  glClearDepth(1);
}

bool InputHandler::should_apply(ecs::Context<Registry> &ctx,
                                ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         ctx.registry().characters.count(id);
}

void InputHandler::update_single(ecs::Context<Registry> &ctx,
                                 ecs::entities::EntityId id) {
  auto &character = ctx.registry().characters[id];
  auto &input_queue = ctx.registry().input_queue;
  while (!input_queue.empty()) {
    const auto input = input_queue.front();
    input_queue.pop();

    switch (input) {
    case InputKind::UP:
      character.actions.push(components::ActionKind::MOVE_UP);
      break;
    case InputKind::DOWN:
      character.actions.push(components::ActionKind::MOVE_DOWN);
      break;
    case InputKind::LEFT:
      character.actions.push(components::ActionKind::MOVE_LEFT);
      break;
    case InputKind::RIGHT:
      character.actions.push(components::ActionKind::MOVE_RIGHT);
      break;
    }
  }
}

bool Character::should_apply(ecs::Context<Registry> &ctx,
                             ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         (ctx.registry().action_restrictions.count(id) ||
          ctx.registry().win_zones.count(id) || ctx.registry().cars.count(id) ||
          ctx.registry().shoe_items.count(id));
}

void Character::pre_update(ecs::Context<Registry> &ctx) {
  ctx.registry().blocked_actions.clear();
}

void Character::post_update(ecs::Context<Registry> &ctx) {
  const auto character_id = ctx.registry().character_id;
  auto &character = ctx.registry().characters[character_id];
  auto &render_info = ctx.registry().render_infos[character_id];
  auto &animation = ctx.registry().animations[character_id];
  if (character.actions.empty() ||
      animation.state == components::AnimationState::RUNNING)
    return;
  else if (animation.state == components::AnimationState::FINISHED) {
    switch (character.current_action) {
    case components::ActionKind::MOVE_UP:
      render_info.mat *=
          glm::translate(glm::mat4(1), glm::vec3(0, STEP_SIZE, 0));
      break;
    case components::ActionKind::MOVE_DOWN:
      render_info.mat *=
          glm::translate(glm::mat4(1), glm::vec3(0, -STEP_SIZE, 0));
      break;
    case components::ActionKind::MOVE_LEFT:
      render_info.mat *=
          glm::translate(glm::mat4(1), glm::vec3(-STEP_SIZE, 0, 0));
      break;
    case components::ActionKind::MOVE_RIGHT:
      render_info.mat *=
          glm::translate(glm::mat4(1), glm::vec3(STEP_SIZE, 0, 0));
      break;
    default:
      break;
    }
    Animation::disable(ctx, character_id);
    Animation::reset(ctx, character_id);
  }

  const auto action = character.actions.front();
  character.actions.pop();
  if (ctx.registry().blocked_actions.count(action))
    return;

  const auto duration = components::Character::DEFAULT_ANIMATION_DURATION /
                        character.speed_multipler;
  switch (action) {
  case components::ActionKind::MOVE_UP:
    Animation::set(ctx, character_id,
                   {components::AnimationKind::ONCE,
                    {{0.0f, glm::mat4(1)},
                     {duration, glm::translate(glm::mat4(1),
                                               glm::vec3(0, STEP_SIZE, 0))}}});
    break;
  case components::ActionKind::MOVE_DOWN:
    Animation::set(ctx, character_id,
                   {components::AnimationKind::ONCE,
                    {{0.0f, glm::mat4(1)},
                     {duration, glm::translate(glm::mat4(1),
                                               glm::vec3(0, -STEP_SIZE, 0))}}});
    break;
  case components::ActionKind::MOVE_LEFT:
    Animation::set(ctx, character_id,
                   {components::AnimationKind::ONCE,
                    {{0.0f, glm::mat4(1)},
                     {duration, glm::translate(glm::mat4(1),
                                               glm::vec3(-STEP_SIZE, 0, 0))}}});
    break;
  case components::ActionKind::MOVE_RIGHT:
    Animation::set(ctx, character_id,
                   {components::AnimationKind::ONCE,
                    {{0.0f, glm::mat4(1)},
                     {duration, glm::translate(glm::mat4(1),
                                               glm::vec3(STEP_SIZE, 0, 0))}}});
    break;
  case components::ActionKind::WEAR_SHOE:
    character.speed_multipler *= components::ShoeItem::MULTIPLIER;
    break;
  }
  character.current_action = action;
}

void Character::update_single(ecs::Context<Registry> &ctx,
                              ecs::entities::EntityId id) {
  const auto &action_restrictions = ctx.registry().action_restrictions;
  const auto &win_zones = ctx.registry().win_zones;
  const auto character_id = ctx.registry().character_id;
  auto &render_infos = ctx.registry().render_infos;
  const auto &character_render_info = render_infos.at(character_id);
  const auto &character_vertices =
      character_render_info.vertex_container->vertices();
  const auto &animation = ctx.registry().animations[character_id];
  const auto character_bb =
      character_render_info.bounding_box_with_trasform(animation.mat);
  auto &shoe_items = ctx.registry().shoe_items;

  if (action_restrictions.count(id)) {
    const auto action_restriction = action_restrictions.at(id);
    if (ctx.registry().pass_through && !action_restriction.ignore_passthrough)
      return;
    if (character_bb.intersect_with(action_restriction.bounding_box)) {
      for (const auto &r : action_restriction.restrictions)
        ctx.registry().blocked_actions.insert(r);
    }
  } else if (win_zones.count(id)) {
    const auto &win_zone = win_zones.at(id);
    if (character_bb.contained_in(win_zone.bounding_box))
      ctx.registry().state = GameState::WIN;
  } else if (shoe_items.count(id)) {
    const auto shoe_item_bb = render_infos.at(id).bounding_box();
    if (!shoe_item_bb.intersect_with(character_bb))
      return;
    render_infos.erase(id);
    shoe_items.erase(id);
    ctx.entity_manager().remove_id(id);
    auto &character = ctx.registry().characters[character_id];
    character.actions.push(components::ActionKind::WEAR_SHOE);
  } else if (!ctx.registry().pass_through) {
    const auto &car_render_info = render_infos.at(id);
    const auto &car_vertices = car_render_info.vertex_container->vertices();
    const auto car_bb = car_render_info.bounding_box();
    if (character_bb.intersect_with(car_bb))
      ctx.registry().state = GameState::LOSE;
  }
}

bool Car::should_apply(ecs::Context<Registry> &ctx,
                       ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         ctx.registry().cars.count(id);
}

void Car::update_single(ecs::Context<Registry> &ctx,
                        ecs::entities::EntityId id) {
  auto &render_infos = ctx.registry().render_infos;
  auto &render_info = render_infos[id];
  auto &car = ctx.registry().cars[id];
  const auto car_bb = render_info.bounding_box();
  const auto xmin = car_bb.top_left[0], xmax = car_bb.bottom_right[0];
  if (car.vel[0] < 0.0f && xmax < -1.0f)
    render_info.mat = glm::translate(render_info.mat, glm::vec3(3.0f, 0, 0));
  else if (car.vel[0] > 0.0f && xmin > 1.0f)
    render_info.mat = glm::translate(render_info.mat, glm::vec3(-3.0f, 0, 0));
  const auto disp = ctx.delta_time() * car.vel;
  render_info.mat = glm::translate(render_info.mat, disp);

  const auto &wheel_ids = ctx.entity_manager().entity_graph()[id].children;
  const auto angle = -disp[0] / WHEEL_RADIUS;
  for (const auto wheel_id : wheel_ids) {
    auto &wheel_render_info = render_infos[wheel_id];
    wheel_render_info.mat =
        glm::rotate(wheel_render_info.mat, angle, glm ::vec3(0, 0, 1));
  }
}

glm::mat4 Animation::interpolate_transforms(float ratio, const glm::mat4 &first,
                                            const glm::mat4 &second) {
  // TODO: use better interpolation algorithms
  return first + ratio * (second - first);
}

bool Animation::should_apply(ecs::Context<Registry> &ctx,
                             ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         ctx.registry().animations.count(id);
}

void Animation::update_single(ecs::Context<Registry> &ctx,
                              ecs::entities::EntityId id) {
  auto &animation = ctx.registry().animations[id];
  const auto &info = animation.info;
  if (info.kind == components::AnimationKind::DISABLED) {
    animation.mat = glm::mat4(1);
    return;
  }

  switch (animation.state) {
  case components::AnimationState::BEFORE_START:
    animation.state = components::AnimationState::RUNNING;
    animation.time_elapsed = 0;
    animation.mat = glm::mat4(1);
    break;
  case components::AnimationState::RUNNING: {
    animation.time_elapsed += ctx.delta_time();
    if (info.kind == components::AnimationKind::LOOP) {
      const auto duration = info.keyframes.crbegin()->first,
                 modulo = std::fmod(animation.time_elapsed, 2 * duration);
      if (modulo > duration)
        animation.time_elapsed = 2 * duration - modulo;
      else
        animation.time_elapsed = modulo;
    }
    const auto time_hi = info.keyframes.upper_bound(animation.time_elapsed);
    if (time_hi == info.keyframes.cbegin() ||
        time_hi == info.keyframes.cend()) {
      animation.state = components::AnimationState::FINISHED;
      break;
    }
    const auto time_lo = std::prev(time_hi);

    const auto ratio = (animation.time_elapsed - time_lo->first) /
                       (time_hi->first - time_lo->first);
    animation.mat =
        interpolate_transforms(ratio, time_lo->second, time_hi->second);
    break;
  }
  case components::AnimationState::FINISHED:
    break;
  }
}

void Animation::set(ecs::Context<Registry> &ctx, ecs::entities::EntityId id,
                    components::AnimationInfo &&animation_info) {
  auto &animation = ctx.registry().animations[id];
  animation.info = animation_info;
  reset(ctx, id);
}

void Animation::reset(ecs::Context<Registry> &ctx, ecs::entities::EntityId id) {
  auto &animation = ctx.registry().animations[id];
  animation.state = components::AnimationState::BEFORE_START;
}

void Animation::disable(ecs::Context<Registry> &ctx,
                        ecs::entities::EntityId id) {
  auto &animation = ctx.registry().animations[id];
  animation.info.kind = components::AnimationKind::DISABLED;
}
} // namespace systems
