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
#include "shader_program.hpp"
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
  if (key == 'r')
    ctx_ptr->registry().hidden_line_removal =
        !ctx_ptr->registry().hidden_line_removal;
  if (key == 'v')
    ctx_ptr->registry().view_mode = (ctx_ptr->registry().view_mode + 1) %
                                    ctx_ptr->registry().camera_config.size();
}

int main(int argc, char **argv) {
#ifndef __APPLE__
  glewInit();
#endif
  glutInit(&argc, argv);
#ifdef __APPLE__
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH |
                      GLUT_3_2_CORE_PROFILE);
#else
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
#endif
  glutInitWindowSize(512, 512);
  glutCreateWindow("Crossy Ponix");

  std::vector<std::shared_ptr<ecs::systems::System<Registry>>> systems;
  systems.emplace_back(new systems::Animation);
  systems.emplace_back(new systems::Render);
  systems.emplace_back(new systems::InputHandler);
  systems.emplace_back(new systems::Character);
  systems.emplace_back(new systems::Car);
  ctx_ptr =
      std::make_shared<ecs::Context<Registry>>(Registry(), std::move(systems));

  ctx_ptr->registry().shader_program =
      ShaderProgram("transform.glsl", "wireframe.glsl");
  glUseProgram(ctx_ptr->registry().shader_program.program_id);

  create_map(*ctx_ptr);

  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard_handle_non_special);
  glutSpecialFunc(keyboard_handle);
  glutMainLoop();
}
