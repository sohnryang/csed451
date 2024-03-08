#include "systems.hpp"

#include "components.hpp"
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
  const auto &registry = ctx.registry();

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

bool InputHandler::should_apply(ecs::Context<Registry> &ctx,
                                ecs::entities::EntityId id) {
  return ctx.registry().characters.count(id);
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
  return ctx.registry().characters.count(id) &&
         ctx.registry().transforms.count(id);
}

void Character::update_single(ecs::Context<Registry> &ctx,
                              ecs::entities::EntityId id) {
  auto &character = ctx.registry().characters[id];
  auto &transform = ctx.registry().transforms[id];
  const float step_size = 2.0f / 8;
  while (!character.actions.empty()) {
    auto action = character.actions.front();
    character.actions.pop();

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
}
} // namespace systems
