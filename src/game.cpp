#include "ecs/systems.hpp"

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#include "registry.hpp"
#include "scene.hpp"
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

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Crossy Ponix");

  input_handler = std::make_shared<systems::InputHandler>();

  std::vector<std::shared_ptr<ecs::systems::System<Registry>>> systems;
  systems.emplace_back(new systems::Animation);
  systems.emplace_back(new systems::Render);
  systems.push_back(input_handler);
  systems.emplace_back(new systems::Character);
  systems.emplace_back(new systems::Car);
  ctx_ptr =
      std::make_shared<ecs::Context<Registry>>(Registry(), std::move(systems));

  create_map(*ctx_ptr);
  create_character(*ctx_ptr);
  create_win_zone(*ctx_ptr);

  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutSpecialFunc(keyboard_handle);
  glutMainLoop();
}
