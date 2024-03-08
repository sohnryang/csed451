#include "ecs/systems.hpp"

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#include "registry.hpp"
#include "systems.hpp"

// TODO: use singleton
std::shared_ptr<systems::InputHandler> input_handler;
std::shared_ptr<ecs::Context<Registry>> ctx_ptr;

void display() { ctx_ptr->update(); }

void idle() { glutPostRedisplay(); }

void keyboard_handle(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_UP:
    input_handler->push_input(systems::InputKind::UP);
    break;
  case GLUT_KEY_DOWN:
    input_handler->push_input(systems::InputKind::DOWN);
    break;
  case GLUT_KEY_LEFT:
    input_handler->push_input(systems::InputKind::LEFT);
    break;
  case GLUT_KEY_RIGHT:
    input_handler->push_input(systems::InputKind::RIGHT);
    break;
  }
}

void create_map() {
  const components::Color grass_color = {68.0 / 255, 132.0 / 255, 46.0 / 255},
                          road_color = {172.0 / 255, 172.0 / 255, 172.0 / 255};

  const float step_size = 2.0f / 8;
  ctx_ptr->registry().add_render_info(
      *ctx_ptr, {{glm::vec4(-step_size * 4, -step_size * 4, 0.0, 1.0),
                  glm::vec4(step_size * 4, -step_size * 4, 0.0, 1.0),
                  glm::vec4(step_size * 4, -step_size * 3, 0.0, 1.0),
                  glm::vec4(-step_size * 4, -step_size * 3, 0.0, 1.0)},
                 grass_color});
  ctx_ptr->registry().add_render_info(
      *ctx_ptr, {{
                     glm::vec4(-step_size * 4, -step_size * 3, 0.0, 1.0),
                     glm::vec4(step_size * 4, -step_size * 3, 0.0, 1.0),
                     glm::vec4(step_size * 4, -step_size * 1, 0.0, 1.0),
                     glm::vec4(-step_size * 4, -step_size * 1, 0.0, 1.0),
                 },
                 road_color});
  ctx_ptr->registry().add_render_info(
      *ctx_ptr, {{glm::vec4(-step_size * 4, -step_size * 1, 0.0, 1.0),
                  glm::vec4(step_size * 4, -step_size * 1, 0.0, 1.0),
                  glm::vec4(step_size * 4, step_size * 0, 0.0, 1.0),
                  glm::vec4(-step_size * 4, step_size * 0, 0.0, 1.0)},
                 grass_color});
  ctx_ptr->registry().add_render_info(
      *ctx_ptr, {{glm::vec4(-step_size * 4, step_size * 0, 0.0, 1.0),
                  glm::vec4(step_size * 4, step_size * 0, 0.0, 1.0),
                  glm::vec4(step_size * 4, step_size * 3, 0.0, 1.0),
                  glm::vec4(-step_size * 4, step_size * 3, 0.0, 1.0)},
                 road_color});
  ctx_ptr->registry().add_render_info(
      *ctx_ptr, {{glm::vec4(-step_size * 4, step_size * 3, 0.0, 1.0),
                  glm::vec4(step_size * 4, step_size * 3, 0.0, 1.0),
                  glm::vec4(step_size * 4, step_size * 4, 0.0, 1.0),
                  glm::vec4(-step_size * 4, step_size * 4, 0.0, 1.0)},
                 grass_color});
}

void create_character() {
  ctx_ptr->registry().characters[ctx_ptr->entity_manager().next_id()] = {};
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Crossy Phonix");

  input_handler = std::make_shared<systems::InputHandler>();

  std::vector<std::shared_ptr<ecs::systems::System<Registry>>> systems;
  systems.emplace_back(new systems::Render);
  systems.push_back(input_handler);
  ctx_ptr =
      std::make_shared<ecs::Context<Registry>>(Registry(), std::move(systems));

  ctx_ptr->registry().add_render_info(*ctx_ptr, {});

  create_map();
  create_character();

  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutSpecialFunc(keyboard_handle);
  glutMainLoop();
}
