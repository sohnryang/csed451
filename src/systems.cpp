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

#include <cstddef>
#include <string>

#include "components.hpp"
#include "registry.hpp"

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
  const auto &registry = ctx.registry();
  const auto &render_info = registry.render_infos.at(id);
  const auto &color = render_info.color;
  glColor3f(color.r, color.g, color.b);

  glPushMatrix();
  if (registry.transforms.count(id)) {
    const auto &transform = registry.transforms.at(id);
    glLoadMatrixf(glm::value_ptr(transform.mat));
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
    transform.mat *= glm::translate(glm::mat4(1), glm::vec3(0, step_size, 0));
    break;
  case components::ActionKind::MOVE_DOWN:
    transform.mat *= glm::translate(glm::mat4(1), glm::vec3(0, -step_size, 0));
    break;
  case components::ActionKind::MOVE_LEFT:
    transform.mat *= glm::translate(glm::mat4(1), glm::vec3(-step_size, 0, 0));
    break;
  case components::ActionKind::MOVE_RIGHT:
    transform.mat *= glm::translate(glm::mat4(1), glm::vec3(step_size, 0, 0));
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
      character_render_info.bounding_box_with_transform(character_transform);

  if (action_restrictions.count(id)) {
    const auto action_restriction = action_restrictions.at(id);
    if (character_bb.intersect_with(action_restriction.bounding_box)) {
      for (const auto &r : action_restriction.restrictions)
        blocked_actions.insert(r);
    }
  } else if (win_zones.count(id)) {
    const auto &win_zone = win_zones.at(id);
    if (character_bb.intersect_with(win_zone.bounding_box))
      ctx.registry().state = GameState::WIN;
  } else {
    const auto &car_render_info = render_infos.at(id);
    const auto &car_vertices = car_render_info.vertices;
    const auto &car_transform = transforms.at(id);
    const auto car_bb =
        car_render_info.bounding_box_with_transform(car_transform);
    if (character_bb.intersect_with(car_bb))
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
  auto &car = ctx.registry().cars[id];
  auto &transform = ctx.registry().transforms.at(id);
  const auto car_bb = render_info.bounding_box_with_transform(transform);
  const auto xmin = car_bb.top_left[0], xmax = car_bb.bottom_right[0];

  if (car.vel[0] < 0.0f && xmax < -1.0f)
    car.disp[0] += 3.0f;
  else if (car.vel[0] > 0.0f && xmin > 1.0f)
    car.disp[0] -= 3.0f;

  car.disp += ctx.delta_time() * car.vel;
  transform.mat = glm::translate(glm::mat4(1), car.disp);
}

Car::Car() {}
} // namespace systems
