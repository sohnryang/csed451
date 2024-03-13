#include "systems.hpp"

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>

#include "components.hpp"
#include "registry.hpp"
#include "utils.hpp"

namespace systems {
bool Render::should_apply(ecs::Context<Registry> &ctx,
                          ecs::entities::EntityId id) {
  return ctx.registry().render_infos.count(id);
}

void Render::pre_update(ecs::Context<Registry> &ctx) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    for (size_t i = 0; i < text.length(); i++) {
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
  const auto &registry = ctx.registry();
  const auto &render_info = registry.render_infos.at(id);
  const auto &color = render_info.color;
  glColor3f(color.r, color.g, color.b);

  glPushMatrix();
  if (registry.transforms.count(id)) {
    const auto &transform = registry.transforms.at(id);
    const glm::mat4 mat = glm::translate(glm::mat4(1), transform.disp);
    glLoadMatrixf(glm::value_ptr(mat));
  } else
    glLoadIdentity();

  glBegin(GL_POLYGON);
  const auto &vertices = render_info.vertices;
  for (const auto &v : vertices)
    glVertex3f(v[0], v[1], v[2]);
  glEnd();
  glPopMatrix();
}

Render::Render() {
  glClearColor(0, 0, 0, 1);
  glDepthFunc(GL_LESS);
  glDepthRange(0, 1);
  glClearDepth(1);
}

bool Transform::should_apply(ecs::Context<Registry> &ctx,
                             ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         ctx.registry().transforms.count(id);
}

void Transform::update_single(ecs::Context<Registry> &ctx,
                              ecs::entities::EntityId id) {
  auto &transforms = ctx.registry().transforms;
  const auto &vel = transforms.at(id).vel;
  auto &disp = transforms[id].disp;
  disp += ctx.delta_time() * vel;
}

bool InputHandler::should_apply(ecs::Context<Registry> &ctx,
                                ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         ctx.registry().characters.count(id);
}

void InputHandler::update_single(ecs::Context<Registry> &ctx,
                                 ecs::entities::EntityId id) {
  auto &character = ctx.registry().characters[id];
  while (!_input_queue.empty()) {
    const auto input = _input_queue.front();
    _input_queue.pop();

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

InputHandler::InputHandler() : _input_queue() {}

void InputHandler::push_input(InputKind input) { _input_queue.push(input); }

bool Character::should_apply(ecs::Context<Registry> &ctx,
                             ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         (ctx.registry().action_restrictions.count(id) ||
          ctx.registry().win_zones.count(id) || ctx.registry().cars.count(id));
}

void Character::pre_update(ecs::Context<Registry> &ctx) {
  blocked_actions.clear();
}

void Character::post_update(ecs::Context<Registry> &ctx) {
  const auto character_id = ctx.registry().character_id;
  auto &character = ctx.registry().characters[character_id];
  auto &transform = ctx.registry().transforms[character_id];
  const float step_size = 2.0f / 8;
  if (character.actions.empty())
    return;

  const auto action = character.actions.front();
  character.actions.pop();
  if (blocked_actions.count(action))
    return;

  switch (action) {
  case components::ActionKind::MOVE_UP:
    transform.disp[1] += step_size;
    break;
  case components::ActionKind::MOVE_DOWN:
    transform.disp[1] -= step_size;
    break;
  case components::ActionKind::MOVE_LEFT:
    transform.disp[0] -= step_size;
    break;
  case components::ActionKind::MOVE_RIGHT:
    transform.disp[0] += step_size;
    break;
  }
}

void Character::update_single(ecs::Context<Registry> &ctx,
                              ecs::entities::EntityId id) {
  const auto &action_restrictions = ctx.registry().action_restrictions;
  const auto &win_zones = ctx.registry().win_zones;
  const auto character_id = ctx.registry().character_id;
  const auto &render_infos = ctx.registry().render_infos;
  const auto &character_render_info = render_infos.at(character_id);
  const auto &character_vertices = character_render_info.vertices;
  const auto &transforms = ctx.registry().transforms;
  const auto &character_transform = transforms.at(character_id);
  const auto character_bb =
      bounding_box(character_render_info, character_transform);

  if (action_restrictions.count(id)) {
    const auto action_restriction = action_restrictions.at(id);
    const auto restriction_bb = BoundingBox(action_restriction.top_left,
                                            action_restriction.bottom_right);
    if (intersect(character_bb, restriction_bb)) {
      for (const auto &r : action_restriction.restrictions)
        blocked_actions.insert(r);
    }
  } else if (win_zones.count(id)) {
    const auto &win_zone = win_zones.at(id);
    const auto win_zone_bb =
        BoundingBox(win_zone.top_left, win_zone.bottom_right);
    if (intersect(character_bb, win_zone_bb))
      ctx.registry().state = GameState::WIN;
  } else {
    const auto &car_render_info = render_infos.at(id);
    const auto &car_vertices = car_render_info.vertices;
    const auto &car_transform = transforms.at(id);
    const auto car_bb = bounding_box(car_render_info, car_transform);
    if (intersect(character_bb, car_bb))
      ctx.registry().state = GameState::LOSE;
  }
}

Character::Character() : blocked_actions() {}

bool Car::should_apply(ecs::Context<Registry> &ctx,
                       ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         ctx.registry().cars.count(id);
}

void Car::update_single(ecs::Context<Registry> &ctx,
                        ecs::entities::EntityId id) {
  const auto &render_info = ctx.registry().render_infos.at(id);
  auto &transform = ctx.registry().transforms.at(id);
  const auto &vertices = render_info.vertices;

  const glm::mat4 mat = glm::translate(glm::mat4(1), transform.disp);
  std::vector<glm::vec4> transformed;
  std::transform(vertices.cbegin(), vertices.cend(),
                 std::back_inserter(transformed),
                 [&mat](const glm::vec4 &vertex) { return mat * vertex; });
  float xmin = transformed[0][0], xmax = transformed[0][0];
  for (const auto &v : transformed) {
    xmin = std::min(xmin, v[0]);
    xmax = std::max(xmax, v[0]);
  }

  if (transform.vel[0] < 0.0f && xmax < -1.0f)
    transform.disp[0] += 3.0f;
  else if (transform.vel[0] > 0.0f && xmin > 1.0f)
    transform.disp[0] -= 3.0f;
}

Car::Car() {}
} // namespace systems
