#include <systems.hpp>

#include <bounding_box.hpp>
#include <ecs/entities.hpp>
#include <ecs/systems.hpp>

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#include <cmath>
#include <cstddef>
#include <iterator>
#include <string>

#include <components.hpp>
#include <grid.hpp>
#include <registry.hpp>
#include <scene.hpp>

namespace systems {
bool Render::should_apply(ecs::Context<Registry> &ctx,
                          ecs::entities::EntityId id) {
  return ctx.registry().meshes.count(id) &&
         ctx.entity_manager().entity_graph()[id].parent == id;
}

void Render::pre_update(ecs::Context<Registry> &ctx) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1, 1, 1);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  if (ctx.registry().hidden_line_removal) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  } else {
    glDisable(GL_CULL_FACE);
  }

  glm::vec3 camera_delta =
      glm::vec3(ctx.registry().meshes[ctx.registry().character_id].mat[3]) +
      glm::vec3(ctx.registry().animations[ctx.registry().character_id].mat[3]) -
      ctx.registry().camera_init;
  // printf("%f %f %f\n", camera_delta[0], camera_delta[1], camera_delta[2]);

  const auto &camera_config = ctx.registry().camera_config;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(camera_config.fovy, camera_config.aspect_ratio,
                 camera_config.znear, camera_config.zfar);
  // glOrtho(-4, 4, -4, 4, camera_config.znear, camera_config.zfar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(camera_config.eye[0], camera_config.eye[1], camera_config.eye[2],
            camera_config.center[0], camera_config.center[1],
            camera_config.center[2], camera_config.up[0], camera_config.up[1],
            camera_config.up[2]);
  glTranslatef(-camera_delta[0], -camera_delta[1], -camera_delta[2]);
  glPushMatrix();
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
  glPopMatrix();
  glutSwapBuffers();
}

void Render::update_single(ecs::Context<Registry> &ctx,
                           ecs::entities::EntityId id) {
  const auto &mesh = ctx.registry().meshes.at(id);
  const auto &animations = ctx.registry().animations;
  glPushMatrix();
  if (animations.count(id))
    glMultMatrixf(glm::value_ptr(mesh.mat * animations.at(id).mat));
  else
    glMultMatrixf(glm::value_ptr(mesh.mat));
  render_single(mesh);
  render_children(ctx, id);
  glPopMatrix();
}

void Render::render_single(const components::Mesh &mesh) {
  glBegin(GL_TRIANGLES);
  for (const auto &v : mesh.vertices)
    glVertex3f(v[0], v[1], v[2]);
  glEnd();
}

void Render::render_children(ecs::Context<Registry> &ctx,
                             ecs::entities::EntityId id) {
  const auto &meshes = ctx.registry().meshes;
  const auto &animations = ctx.registry().animations;

  float matrix_raw[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix_raw);
  const auto base_mat = glm::make_mat4(matrix_raw);

  for (const auto child_id : ctx.entity_manager().entity_graph()[id].children) {
    if (!meshes.count(child_id))
      continue;

    glPushMatrix();
    const auto &mesh = meshes.at(child_id);
    const auto child_mat = base_mat * mesh.mat;
    if (animations.count(child_id))
      glLoadMatrixf(glm::value_ptr(child_mat * animations.at(child_id).mat));
    else
      glLoadMatrixf(glm::value_ptr(child_mat));
    render_single(mesh);
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
      character.actions.push(components::ActionKind::MOVE_FORWARD);
      break;
    case InputKind::DOWN:
      character.actions.push(components::ActionKind::MOVE_BACK);
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
  auto &mesh = ctx.registry().meshes[character_id];
  auto &animation = ctx.registry().animations[character_id];
  if (animation.state == components::AnimationState::FINISHED) {
    switch (character.current_action) {
    case components::ActionKind::MOVE_FORWARD:
      mesh.mat *= glm::translate(glm::mat4(1), glm::vec3(0, 0, -STEP_SIZE));
      break;
    case components::ActionKind::MOVE_BACK:
      mesh.mat *= glm::translate(glm::mat4(1), glm::vec3(0, 0, STEP_SIZE));
      break;
    case components::ActionKind::MOVE_LEFT:
      mesh.mat *= glm::translate(glm::mat4(1), glm::vec3(-STEP_SIZE, 0, 0));
      break;
    case components::ActionKind::MOVE_RIGHT:
      mesh.mat *= glm::translate(glm::mat4(1), glm::vec3(STEP_SIZE, 0, 0));
      break;
    default:
      break;
    }
    Animation::disable(ctx, character_id);
    Animation::reset(ctx, character_id);
  }
  if (character.actions.empty() ||
      animation.state == components::AnimationState::RUNNING)
    return;

  const auto action = character.actions.front();
  character.actions.pop();
  if (ctx.registry().blocked_actions.count(action))
    return;

  const auto duration = components::Character::DEFAULT_ANIMATION_DURATION /
                        character.speed_multipler;
  switch (action) {
  case components::ActionKind::MOVE_FORWARD:
    Animation::set(ctx, character_id,
                   {components::AnimationKind::ONCE,
                    {{0.0f, glm::mat4(1)},
                     {duration, glm::translate(glm::mat4(1),
                                               glm::vec3(0, 0, -STEP_SIZE))}}});
    break;
  case components::ActionKind::MOVE_BACK:
    Animation::set(ctx, character_id,
                   {components::AnimationKind::ONCE,
                    {{0.0f, glm::mat4(1)},
                     {duration, glm::translate(glm::mat4(1),
                                               glm::vec3(0, 0, STEP_SIZE))}}});
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
    for (const auto child_id :
         ctx.entity_manager().entity_graph()[character_id].children) {
      if (!ctx.registry().animations.count(child_id))
        continue;
      auto &child_animation = ctx.registry().animations[child_id];
      std::map<float, glm::mat4> compressed;
      for (const auto &p : child_animation.info.keyframes)
        compressed[p.first / components::ShoeItem::MULTIPLIER] = p.second;
      Animation::set(ctx, child_id, {child_animation.info.kind, compressed});
    }
    break;
  }
  if (action != components::ActionKind::WEAR_SHOE)
    character.current_action = action;
}

void Character::update_single(ecs::Context<Registry> &ctx,
                              ecs::entities::EntityId id) {
  const auto &action_restrictions = ctx.registry().action_restrictions;
  const auto &win_zones = ctx.registry().win_zones;
  const auto character_id = ctx.registry().character_id;
  auto &meshes = ctx.registry().meshes;
  const auto &character_mesh = meshes.at(character_id);
  const auto &character_vertices = character_mesh.vertices;
  const auto &animation = ctx.registry().animations[character_id];
  const auto character_center =
      character_mesh.mat * animation.mat * glm::vec4(0, 0, 0, 1);
  const auto character_bb =
      ctx.registry().characters[character_id].model_bb.transform(
          character_mesh.mat * animation.mat);
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
    const auto &shoe_item = shoe_items[id];
    const auto &shoe_mesh = meshes.at(id);
    const auto shoe_item_bb = shoe_item.model_bb.transform(shoe_mesh.mat);
    if (!shoe_item_bb.intersect_with(character_bb))
      return;
    meshes.erase(id);
    shoe_items.erase(id);
    ctx.entity_manager().remove_id(id);
    auto &character = ctx.registry().characters[character_id];
    character.actions.push(components::ActionKind::WEAR_SHOE);
  } else if (!ctx.registry().pass_through) {
    const auto &car = ctx.registry().cars[id];
    const auto &car_mesh = meshes.at(id);
    const auto &car_vertices = car_mesh.vertices;
    const auto car_bb = car.model_bb.transform(car_mesh.mat);
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
  auto &meshes = ctx.registry().meshes;
  auto &mesh = meshes[id];
  auto &car = ctx.registry().cars[id];
  const auto car_bb = car.model_bb.transform(mesh.mat);
  const auto xmin = car_bb.min_point[0], xmax = car_bb.max_point[0];
  if (car.vel[0] < 0.0f && xmax < -STEP_SIZE * GRID_SIZE / 2 - STEP_SIZE)
    mesh.mat =
        glm::translate(glm::mat4(1),
                       glm::vec3(GRID_SIZE * STEP_SIZE + STEP_SIZE * 3, 0, 0)) *
        mesh.mat;
  else if (car.vel[0] > 0.0f && xmin > STEP_SIZE * GRID_SIZE / 2 + STEP_SIZE)
    mesh.mat = glm::translate(
                   glm::mat4(1),
                   glm::vec3(-3.0f * STEP_SIZE - GRID_SIZE * STEP_SIZE, 0, 0)) *
               mesh.mat;
  const auto disp = ctx.delta_time() * car.vel;
  mesh.mat = glm::translate(glm::mat4(1), disp) * mesh.mat;
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
    auto normalized_time = animation.time_elapsed;
    if (info.kind == components::AnimationKind::LOOP) {
      const auto duration = info.keyframes.crbegin()->first,
                 modulo = std::fmod(animation.time_elapsed, 2 * duration);
      if (modulo > duration)
        normalized_time = 2 * duration - modulo;
      else
        normalized_time = modulo;
    }
    const auto time_hi = info.keyframes.upper_bound(normalized_time);
    if (time_hi == info.keyframes.cbegin() ||
        time_hi == info.keyframes.cend()) {
      animation.state = components::AnimationState::FINISHED;
      break;
    }
    const auto time_lo = std::prev(time_hi);

    const auto ratio =
        (normalized_time - time_lo->first) / (time_hi->first - time_lo->first);
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
