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

// TODO: map generation
// divide the map with NxN grid. let every entity placement, movement and collision check can be done by those grid
// row 0 (start) and row N-1 (end) is fixed to be a ground, the other are randomly chosen to be a ground or a road
// on the ground row, place woods at randomly chosen 0 ~ N/2 columns
// on the road row, randomly choose the direction and speed of the cars
// draw dotted line between continuous road rows

const size_t grid_size = 8;
const double step_size = 2.0f / grid_size;

void fill_map_row(size_t row_index, const components::Color& color) {
    ctx_ptr->registry().add_render_info(
        *ctx_ptr, {{glm::vec4(-1.0, -1.0 + step_size * row_index, 0.0, 1.0),
                    glm::vec4(1.0, -1.0 + step_size * row_index, 0.0, 1.0),
                    glm::vec4(1.0, -1.0 + step_size * (row_index + 1), 0.0, 1.0),
                    glm::vec4(-1.0,-1.0 + step_size * (row_index + 1), 0.0, 1.0)},
                   color });
}

void create_tree(size_t row_index, size_t col_index, const components::Color& color) {
    // might move this constant (0.75) to somewhere
    const double tree_size = step_size * 0.75;
    const double actual_pos_x = -1.0 + step_size * col_index;
    const double actual_pos_y = -1.0 + step_size * row_index;
    const double gap = (step_size - tree_size) / 2.0;
    ctx_ptr->registry().add_render_info(
        *ctx_ptr, { {glm::vec4(actual_pos_x + gap, actual_pos_y + gap, 1.0, 1.0),
                    glm::vec4(actual_pos_x + gap + tree_size, actual_pos_y + gap, 1.0, 1.0),
                    glm::vec4(actual_pos_x + gap + tree_size, actual_pos_y + gap + tree_size, 1.0, 1.0),
                    glm::vec4(actual_pos_x + gap, actual_pos_y + gap + tree_size, 1.0, 1.0)},
                   color });

    // TODO: add tree context
}

void create_map() {
  const components::Color grass_color = {68.0 / 255, 132.0 / 255, 46.0 / 255},
                          road_color = {172.0 / 255, 172.0 / 255, 172.0 / 255},
                          tree_color = {200.0 / 255, 131.0 / 255, 0.0 / 255};

  fill_map_row(0, grass_color);
  fill_map_row(1, road_color);
  fill_map_row(2, road_color);
  fill_map_row(3, grass_color);
  fill_map_row(4, road_color);
  fill_map_row(5, road_color);
  fill_map_row(6, road_color);
  fill_map_row(7, grass_color);

  create_tree(3, 0, tree_color);
  create_tree(3, 3, tree_color);
  create_tree(3, 6, tree_color);
  create_tree(7, 2, tree_color);
  create_tree(7, 3, tree_color);
  create_tree(7, 5, tree_color);
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