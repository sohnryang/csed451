#include "ecs/systems.hpp"

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#ifdef __APPLE__
#include <OpenGL/gl3.h>

#define __gl_h_
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "registry.hpp"
#include "scene.hpp"
#include "systems.hpp"

// TODO: use singleton
std::shared_ptr<ecs::Context<Registry>> ctx_ptr;

void display() { ctx_ptr->update(); }

void idle() { glutPostRedisplay(); }

void keyboard_handle(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_UP:
    ctx_ptr->registry().input_queue.push(InputKind::UP);
    break;
  case GLUT_KEY_DOWN:
    ctx_ptr->registry().input_queue.push(InputKind::DOWN);
    break;
  case GLUT_KEY_LEFT:
    ctx_ptr->registry().input_queue.push(InputKind::LEFT);
    break;
  case GLUT_KEY_RIGHT:
    ctx_ptr->registry().input_queue.push(InputKind::RIGHT);
    break;
  }
}

void keyboard_handle_non_special(unsigned char key, int x, int y) {
  if (key == 'p')
    ctx_ptr->registry().pass_through = !ctx_ptr->registry().pass_through;

  if (key == 'v')
    ctx_ptr->registry().view_mode = (ctx_ptr->registry().view_mode + 1) %
                                    ctx_ptr->registry().camera_config.size();
  if (key == 'x')
    ctx_ptr->registry().program_index =
        (ctx_ptr->registry().program_index + 1) %
        ctx_ptr->registry().shader_programs.size();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
#ifdef __APPLE__
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH |
                      GLUT_3_2_CORE_PROFILE);
#else
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
#endif
  glutInitWindowSize(512, 512);
  glutCreateWindow("Crossy Ponix");

#ifndef __APPLE__
  glewInit();
#endif

  std::vector<std::shared_ptr<ecs::systems::System<Registry>>> systems;
  systems.emplace_back(new systems::Animation);
  systems.emplace_back(new systems::Render);
  systems.emplace_back(new systems::InputHandler);
  systems.emplace_back(new systems::Character);
  systems.emplace_back(new systems::Car);
  ctx_ptr =
      std::make_shared<ecs::Context<Registry>>(Registry(), std::move(systems));

  glClearColor(0, 0, 0, 1);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0, 1);
  glClearDepth(1);
  glEnable(GL_DEPTH_TEST);

  create_map_init(*ctx_ptr);
  while (ctx_ptr->registry().map_top_generated <= 24)
    create_map(*ctx_ptr);

  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard_handle_non_special);
  glutSpecialFunc(keyboard_handle);
  glutMainLoop();
}
