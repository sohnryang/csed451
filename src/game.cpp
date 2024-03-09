#include "components.hpp"
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
// divide the map with NxN grid. let every entity placement, movement and
// collision check can be done by those grid row 0 (start) and row N-1 (end) is
// fixed to be a ground, the other are randomly chosen to be a ground or a road
// on the ground row, place woods at randomly chosen 0 ~ N/2 columns
// on the road row, randomly choose the direction and speed of the cars
// draw dotted line between continuous road rows

const size_t grid_size = 8;
const float step_size = 2.0f / grid_size;

void fill_map_row(size_t row_index, const components::Color &color) {
  ctx_ptr->registry().add_render_info(
      *ctx_ptr,
      {{glm::vec4(-1.0, -1.0 + step_size * row_index, 0.0, 1.0),
        glm::vec4(1.0, -1.0 + step_size * row_index, 0.0, 1.0),
        glm::vec4(1.0, -1.0 + step_size * (row_index + 1), 0.0, 1.0),
        glm::vec4(-1.0, -1.0 + step_size * (row_index + 1), 0.0, 1.0)},
       color});
}

void create_tree(size_t row_index, size_t col_index,
                 const components::Color &color) {
  const auto id = ctx_ptr->entity_manager().next_id();
  // might move this constant (0.75) to somewhere
  const float tree_radius = step_size * 0.75f / 2.0f;
  const float actual_pos_x = -1.0 + step_size * col_index + step_size * 0.5f;
  const float actual_pos_y = -1.0 + step_size * row_index + step_size * 0.5f;
  ctx_ptr->registry().render_infos[id] = {
      {glm::vec4(-tree_radius, -tree_radius, 0.5f, 1.0f),
       glm::vec4(tree_radius, -tree_radius, 0.5f, 1.0f),
       glm::vec4(tree_radius, tree_radius, 0.5f, 1.0f),
       glm::vec4(-tree_radius, tree_radius, 0.5f, 1.0f)},
      color};
  ctx_ptr->registry().transforms[id] = {
      glm::vec3(actual_pos_x, actual_pos_y, 0.0f), glm::vec3(0)};

  std::vector<std::pair<glm::vec2, components::ActionKind>> adjacent_pos = {
      {{-1, 0}, components::ActionKind::MOVE_RIGHT},
      {{1, 0}, components::ActionKind::MOVE_LEFT},
      {{0, -1}, components::ActionKind::MOVE_UP},
      {{0, 1}, components::ActionKind::MOVE_DOWN}};
  for (const auto &p : adjacent_pos) {
    const auto delta = p.first;
    const auto action = p.second;
    const auto rect_center =
        step_size * delta + glm::vec2(actual_pos_x, actual_pos_y);
    const auto diagonal = glm::vec2(tree_radius, -tree_radius);
    const auto top_left = rect_center - diagonal;
    const auto bottom_right = rect_center + diagonal;
    const auto restriction_id = ctx_ptr->entity_manager().next_id();

    ctx_ptr->registry().action_restrictions[restriction_id] = {
        top_left, bottom_right, {action}};
  }
}

void create_road_line(const size_t row_index, const components::Color &color) {
  const float line_width = step_size * 0.05f;
  const float pos_y = -1.0f + step_size * (row_index + 1) - line_width / 2.0;
  // TODO: randomize initial pos_x value?
  for (float pos_x = -1.06; pos_x < 1.0; pos_x += step_size * 0.87) {
    ctx_ptr->registry().add_render_info(
        *ctx_ptr,
        {{glm::vec4(pos_x, pos_y, 0.1, 1.0),
          glm::vec4(pos_x + step_size * 0.6, pos_y, 0.1, 1.0),
          glm::vec4(pos_x + step_size * 0.6, pos_y + line_width, 0.1, 1.0),
          glm::vec4(pos_x, pos_y + line_width, 0.1, 1.0)},
         color});
  }
}

void create_car(const float pos_x, const size_t row_index, const float vel,
                const components::Color &color) {
  const auto id = ctx_ptr->entity_manager().next_id();
  // might move this constant (0.75) to somewhere
  const float car_radius_x = step_size * 1.5f / 2.0f;
  const float car_radius_y = step_size * 0.7f / 2.0f;
  const float actual_pos_y = -1.0 + step_size * row_index + step_size * 0.5f;
  ctx_ptr->registry().render_infos[id] = {
      {glm::vec4(-car_radius_x, -car_radius_y, 0.5f, 1.0f),
       glm::vec4(car_radius_x, -car_radius_y, 0.5f, 1.0f),
       glm::vec4(car_radius_x, car_radius_y, 0.5f, 1.0f),
       glm::vec4(-car_radius_x, car_radius_y, 0.5f, 1.0f)},
      color};
  ctx_ptr->registry().transforms[id] = {glm::vec3(pos_x, actual_pos_y, 0.0f),
                                        glm::vec3(vel, 0.0, 0.0)};

  ctx_ptr->registry().cars[id] = {};
}

void create_map() {
  const components::Color grass_color = {68.0 / 255, 132.0 / 255, 46.0 / 255},
                          road_color = {172.0 / 255, 172.0 / 255, 172.0 / 255},
                          tree_color = {200.0 / 255, 131.0 / 255, 0.0 / 255},
                          road_line_color = {255.0 / 255, 255.0 / 255,
                                             255.0 / 255},
                          car_color = {66.0 / 255, 147.0 / 255, 252.0 / 255};

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

  create_road_line(1, road_line_color);
  create_road_line(4, road_line_color);
  create_road_line(5, road_line_color);

  create_car(-0.7f, 1, 0.2f, car_color);
  create_car(0.1f, 1, 0.2f, car_color);
  create_car(-0.3f, 2, -0.3f, car_color);

  std::vector<std::tuple<glm::vec2, glm::vec2, components::ActionKind>>
      adjacent_pos = {{{1.0f - step_size, 1.0f},
                       {1.0f, -1.0f},
                       components::ActionKind::MOVE_RIGHT},
                      {{-1.0f, 1.0f},
                       {-1.0f + step_size, -1.0f},
                       components::ActionKind::MOVE_LEFT},
                      {{-1.0f, 1.0f},
                       {1.0f, 1.0f - step_size},
                       components::ActionKind::MOVE_UP},
                      {{-1.0f, -1.0f + step_size},
                       {1.0f, -1.0f},
                       components::ActionKind::MOVE_DOWN}};
  for (const auto &p : adjacent_pos) {
    const auto restriction_id = ctx_ptr->entity_manager().next_id();
    ctx_ptr->registry().action_restrictions[restriction_id] = {
        std::get<0>(p), std::get<1>(p), {std::get<2>(p)}};
  }
}

void create_character() {
  const auto id = ctx_ptr->entity_manager().next_id();
  const auto character_radius = 0.1f;
  const components::Color color = {1.0f, 1.0f, 1.0f};
  ctx_ptr->registry().characters[id] = {};
  ctx_ptr->registry().render_infos[id] = {
      {glm::vec4(-character_radius, -character_radius, 0.5f, 1.0f),
       glm::vec4(character_radius, -character_radius, 0.5f, 1.0f),
       glm::vec4(character_radius, character_radius, 0.5f, 1.0f),
       glm::vec4(-character_radius, character_radius, 0.5f, 1.0f)},
      color};
  ctx_ptr->registry().transforms[id] = {glm::vec3(0.125f, -0.875f, 0.0f),
                                        glm::vec3(0)};
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Crossy Phonix");

  input_handler = std::make_shared<systems::InputHandler>();

  std::vector<std::shared_ptr<ecs::systems::System<Registry>>> systems;
  systems.emplace_back(new systems::Render);
  systems.emplace_back(new systems::Transform);
  systems.push_back(input_handler);
  systems.emplace_back(new systems::Character);
  systems.emplace_back(new systems::Car);
  ctx_ptr =
      std::make_shared<ecs::Context<Registry>>(Registry(), std::move(systems));

  create_map();
  create_character();

  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutSpecialFunc(keyboard_handle);
  glutMainLoop();
}
