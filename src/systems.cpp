#include "systems.hpp"

#include "components.hpp"
#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <algorithm>
#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#include <iterator>

#include "registry.hpp"

namespace systems {
bool Render::should_apply(ecs::Context<Registry> &ctx,
                          ecs::entities::EntityId id) {
  return ctx.registry().render_infos.count(id);
}

void Render::pre_update(ecs::Context<Registry> &ctx) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::post_update(ecs::Context<Registry> &ctx) { glutSwapBuffers(); }

void Render::update_single(ecs::Context<Registry> &ctx,
                           ecs::entities::EntityId id) {
  auto &registry = ctx.registry();

  const auto &render_info = registry.render_infos.at(id);
  const auto &color = render_info.color;
  glColor3f(color.r, color.g, color.b);

  glPushMatrix();
  if (registry.transforms.count(id)) {
    const auto &transform = registry.transforms.at(id);
    glm::mat4 mat = glm::translate(glm::mat4(1), transform.disp);
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
  // TODO: enforce one character limit
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
          ctx.registry().characters.count(id) &&
              ctx.registry().transforms.count(id) &&
              ctx.registry().render_infos.count(id));
}

void Character::pre_update(ecs::Context<Registry> &ctx) {
  blocked_actions.clear();
}

void Character::post_update(ecs::Context<Registry> &ctx) {
  if (!character_found)
    return;

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
  case components::ActionKind::LOSE:
    ctx.registry().state = GameState::LOSE;
    break;
  case components::ActionKind::WIN:
    ctx.registry().state = GameState::WIN;
    break;
  }
}

void Character::update_single(ecs::Context<Registry> &ctx,
                              ecs::entities::EntityId id) {
  const auto &action_restrictions = ctx.registry().action_restrictions;
  const auto &characters = ctx.registry().characters;
  if (character_found && action_restrictions.count(id)) {
    const auto &render_info = ctx.registry().render_infos.at(character_id);
    const auto &vertices = render_info.vertices;
    const auto &transform = ctx.registry().transforms.at(character_id);

    const glm::mat4 mat = glm::translate(glm::mat4(1), transform.disp);
    std::vector<glm::vec4> transformed;
    std::transform(vertices.cbegin(), vertices.cend(),
                   std::back_inserter(transformed),
                   [&mat](const glm::vec4 &vertex) { return mat * vertex; });
    float xmin = transformed[0][0], xmax = transformed[0][0],
          ymin = transformed[0][1], ymax = transformed[0][1];
    for (const auto &v : transformed) {
      xmin = std::min(xmin, v[0]);
      xmax = std::max(xmax, v[0]);
      ymin = std::min(ymin, v[1]);
      ymax = std::max(ymax, v[1]);
    }

    const auto action_restriction = action_restrictions.at(id);
    if (intersect(glm::vec2(xmin, ymax), glm::vec2(xmax, ymin),
                  action_restriction.top_left,
                  action_restriction.bottom_right)) {
      for (const auto &r : action_restriction.restrictions)
        blocked_actions.insert(r);
    }
  } else if (!character_found && characters.count(id)) {
    character_found = true;
    character_id = id;
  }
}

bool Character::intersect(glm::vec2 top_left1, glm::vec2 bottom_right1,
                          glm::vec2 top_left2, glm::vec2 bottom_right2) {
  return top_left1[0] <= bottom_right2[0] && top_left2[0] <= bottom_right1[0] &&
         bottom_right1[1] <= top_left2[1] && bottom_right2[1] <= top_left1[1];
}

Character::Character()
    : character_found(false), character_id(0), blocked_actions() {}

bool Car::should_apply(ecs::Context<Registry> &ctx,
                       ecs::entities::EntityId id) {
  return ctx.registry().state == GameState::IN_PROGRESS &&
         ctx.registry().cars.count(id);
}

void Car::update_single(ecs::Context<Registry> &ctx,
                        ecs::entities::EntityId id) {
  auto &render_info = ctx.registry().render_infos.at(id);
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