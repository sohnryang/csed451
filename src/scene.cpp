#include "scene.hpp"

#include "ecs/systems.hpp"

#include <cstddef>
#include <vector>

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include "components.hpp"
#include "grid.hpp"
#include "registry.hpp"

// TODO: map generation
// divide the map with NxN grid. let every entity placement, movement and
// collision check can be done by those grid row 0 (start) and row N-1 (end) is
// fixed to be a ground, the other are randomly chosen to be a ground or a road
// on the ground row, place woods at randomly chosen 0 ~ N/2 columns
// on the road row, randomly choose the direction and speed of the cars
// draw dotted line between continuous road rows

void fill_map_row(ecs::Context<Registry> &ctx, std::size_t row_index,
                  const components::Color &color) {
  const auto bottom_left = grid_to_world(0, row_index),
             top_right = grid_to_world(8, row_index + 1);
  ctx.registry().add_render_info(
      ctx, {{glm::vec4(bottom_left[0], bottom_left[1], 0.0, 1.0),
             glm::vec4(top_right[0], bottom_left[1], 0.0, 1.0),
             glm::vec4(top_right[0], top_right[1], 0.0, 1.0),
             glm::vec4(bottom_left[0], top_right[1], 0.0, 1.0)},
            color,
            glm::mat4(1)});
}

void create_tree(ecs::Context<Registry> &ctx, std::size_t row_index,
                 std::size_t col_index, const components::Color &color) {
  // might move this constant (0.75) to somewhere
  const float tree_radius = step_size * 0.75f / 2.0f;
  const auto tree_pos = grid_to_world_cell(col_index, row_index).midpoint();
  const auto id = ctx.registry().add_render_info(
      ctx,
      {{glm::vec4(-tree_radius, -tree_radius, 0.5f, 1.0f),
        glm::vec4(tree_radius, -tree_radius, 0.5f, 1.0f),
        glm::vec4(tree_radius, tree_radius, 0.5f, 1.0f),
        glm::vec4(-tree_radius, tree_radius, 0.5f, 1.0f)},
       color,
       glm::translate(glm::mat4(1), glm::vec3(tree_pos[0], tree_pos[1], 0))});

  const std::vector<std::pair<glm::vec2, components::ActionKind>> adjacent_pos =
      {{{-1, 0}, components::ActionKind::MOVE_RIGHT},
       {{1, 0}, components::ActionKind::MOVE_LEFT},
       {{0, -1}, components::ActionKind::MOVE_UP},
       {{0, 1}, components::ActionKind::MOVE_DOWN}};
  for (const auto &p : adjacent_pos) {
    const auto delta = p.first;
    const auto action = p.second;
    const auto rect_center = step_size * delta + tree_pos;
    const auto diagonal = glm::vec2(tree_radius, -tree_radius);
    const auto top_left = rect_center - diagonal;
    const auto bottom_right = rect_center + diagonal;
    const auto restriction_id = ctx.entity_manager().next_id();

    ctx.registry().action_restrictions[restriction_id] = {
        {top_left, bottom_right}, {action}};
  }
}

void create_road_line(ecs::Context<Registry> &ctx, const std::size_t row_index,
                      const components::Color &color) {
  const float line_width = step_size * 0.05f;
  const float pos_y = -1.0f + step_size * (row_index + 1) - line_width / 2.0;
  // TODO: randomize initial pos_x value?
  for (float pos_x = -1.06; pos_x < 1.0; pos_x += step_size * 0.87) {
    ctx.registry().add_render_info(
        ctx, {{glm::vec4(pos_x, pos_y, 0.1, 1.0),
               glm::vec4(pos_x + step_size * 0.6, pos_y, 0.1, 1.0),
               glm::vec4(pos_x + step_size * 0.6, pos_y + line_width, 0.1, 1.0),
               glm::vec4(pos_x, pos_y + line_width, 0.1, 1.0)},
              color,
              glm::mat4(1)});
  }
}

void create_car(ecs::Context<Registry> &ctx, const float pos_x,
                const std::size_t row_index, const float vel,
                const components::Color &color) {
  // might move this constant (0.75) to somewhere
  const float car_radius_x = step_size * 1.5f / 2.0f;
  const float car_radius_y = step_size * 0.7f / 2.0f;
  const float actual_pos_y = -1.0 + step_size * row_index + step_size * 0.5f;
  const auto id = ctx.registry().add_render_info(
      ctx,
      {{glm::vec4(-car_radius_x, -car_radius_y, 0.5f, 1.0f),
        glm::vec4(car_radius_x, -car_radius_y, 0.5f, 1.0f),
        glm::vec4(car_radius_x, car_radius_y, 0.5f, 1.0f),
        glm::vec4(-car_radius_x, car_radius_y, 0.5f, 1.0f)},
       color,
       glm::translate(glm::mat4(1), glm::vec3(pos_x, actual_pos_y, 0.0f))});
  ctx.registry().cars[id] = {glm::vec3(vel, 0.0, 0.0)};
}

void create_map(ecs::Context<Registry> &ctx) {
  const components::Color grass_color = {68.0 / 255, 132.0 / 255, 46.0 / 255},
                          road_color = {172.0 / 255, 172.0 / 255, 172.0 / 255},
                          tree_color = {200.0 / 255, 131.0 / 255, 0.0 / 255},
                          road_line_color = {255.0 / 255, 255.0 / 255,
                                             255.0 / 255},
                          car_color = {66.0 / 255, 147.0 / 255, 252.0 / 255};

  fill_map_row(ctx, 0, grass_color);
  fill_map_row(ctx, 1, road_color);
  fill_map_row(ctx, 2, road_color);
  fill_map_row(ctx, 3, grass_color);
  fill_map_row(ctx, 4, road_color);
  fill_map_row(ctx, 5, road_color);
  fill_map_row(ctx, 6, road_color);
  fill_map_row(ctx, 7, grass_color);

  create_tree(ctx, 3, 0, tree_color);
  create_tree(ctx, 3, 3, tree_color);
  create_tree(ctx, 3, 6, tree_color);
  create_tree(ctx, 7, 2, tree_color);
  create_tree(ctx, 7, 3, tree_color);
  create_tree(ctx, 7, 5, tree_color);

  create_road_line(ctx, 1, road_line_color);
  create_road_line(ctx, 4, road_line_color);
  create_road_line(ctx, 5, road_line_color);

  create_car(ctx, -0.7f, 1, 0.2f, car_color);
  create_car(ctx, 0.1f, 1, 0.2f, car_color);
  create_car(ctx, -0.3f, 2, -0.3f, car_color);
  create_car(ctx, 0.1f, 2, -0.3f, car_color);
  create_car(ctx, -0.4f, 4, 0.25f, car_color);
  create_car(ctx, 0.6f, 4, 0.25f, car_color);
  create_car(ctx, -1.0f, 5, 0.35f, car_color);
  create_car(ctx, 0.2f, 5, 0.35f, car_color);
  create_car(ctx, 0.7f, 5, 0.35f, car_color);
  create_car(ctx, -0.8f, 6, -0.15f, car_color);
  create_car(ctx, -0.1f, 6, -0.15f, car_color);
  create_car(ctx, 0.7f, 6, -0.15f, car_color);

  const std::vector<std::pair<BoundingBox, components::ActionKind>>
      adjacent_pos = {{{
                           {1.0f - step_size, 1.0f},
                           {1.0f, -1.0f},
                       },
                       components::ActionKind::MOVE_RIGHT},
                      {{{-1.0f, 1.0f}, {-1.0f + step_size, -1.0f}},
                       components::ActionKind::MOVE_LEFT},
                      {{{-1.0f, 1.0f}, {1.0f, 1.0f - step_size}},
                       components::ActionKind::MOVE_UP},
                      {{{-1.0f, -1.0f + step_size}, {1.0f, -1.0f}},
                       components::ActionKind::MOVE_DOWN}};
  for (const auto &p : adjacent_pos) {
    const auto restriction_id = ctx.entity_manager().next_id();
    const auto &bb = p.first;
    const auto &action = p.second;
    ctx.registry().action_restrictions[restriction_id] = {bb, {action}};
  }
}

void create_character(ecs::Context<Registry> &ctx) {
  const auto character_radius = 0.1f;
  const components::Color color = {1.0f, 1.0f, 1.0f};
  const auto character_pos = grid_to_world_cell(4, 0).midpoint();
  const auto id = ctx.registry().add_render_info(
      ctx, {{glm::vec4(-character_radius, -character_radius, 0.5f, 1.0f),
             glm::vec4(character_radius, -character_radius, 0.5f, 1.0f),
             glm::vec4(character_radius, character_radius, 0.5f, 1.0f),
             glm::vec4(-character_radius, character_radius, 0.5f, 1.0f)},
            color,
            glm::translate(glm::mat4(1), glm::vec3(character_pos[0],
                                                   character_pos[1], 0.0f))});
  ctx.registry().characters[id] = {};
  ctx.registry().character_id = id;
}

void create_win_zone(ecs::Context<Registry> &ctx) {
  const auto id = ctx.entity_manager().next_id();
  ctx.registry().win_zones[id] = {
      {glm::vec2(-1, 1), glm::vec2(1, 1 - step_size)}};
}
