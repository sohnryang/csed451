#include "scene.hpp"

#include "ecs/systems.hpp"

#include <cstddef>
#include <memory>
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
  std::vector<glm::vec4> vertices = {
      glm::vec4(bottom_left[0], bottom_left[1], 0.0, 1.0),
      glm::vec4(top_right[0], bottom_left[1], 0.0, 1.0),
      glm::vec4(top_right[0], top_right[1], 0.0, 1.0),
      glm::vec4(bottom_left[0], top_right[1], 0.0, 1.0)};
  ctx.registry().add_render_info(
      ctx, {std::make_unique<components::VertexVector>(std::move(vertices)),
            color, glm::mat4(1)});
}

void create_tree(ecs::Context<Registry> &ctx, std::size_t row_index,
                 std::size_t col_index, const components::Color &color) {
  // might move this constant (0.75) to somewhere
  const auto tree_pos = grid_to_world_cell(col_index, row_index).midpoint();
  std::vector<glm::vec4> vertices = {
      glm::vec4(-TREE_RADIUS, -TREE_RADIUS, 0.5f, 1.0f),
      glm::vec4(TREE_RADIUS, -TREE_RADIUS, 0.5f, 1.0f),
      glm::vec4(TREE_RADIUS, TREE_RADIUS, 0.5f, 1.0f),
      glm::vec4(-TREE_RADIUS, TREE_RADIUS, 0.5f, 1.0f)};
  const auto id = ctx.registry().add_render_info(
      ctx,
      {std::make_unique<components::VertexVector>(std::move(vertices)), color,
       glm::translate(glm::mat4(1), glm::vec3(tree_pos[0], tree_pos[1], 0))});

  const std::vector<std::pair<glm::vec2, components::ActionKind>> adjacent_pos =
      {{{-1, 0}, components::ActionKind::MOVE_RIGHT},
       {{1, 0}, components::ActionKind::MOVE_LEFT},
       {{0, -1}, components::ActionKind::MOVE_UP},
       {{0, 1}, components::ActionKind::MOVE_DOWN}};
  for (const auto &p : adjacent_pos) {
    const auto delta = p.first;
    const auto action = p.second;
    const auto rect_center = STEP_SIZE * delta + tree_pos;
    const auto diagonal = glm::vec2(TREE_RADIUS, -TREE_RADIUS);
    const auto top_left = rect_center - diagonal;
    const auto bottom_right = rect_center + diagonal;
    const auto restriction_id = ctx.entity_manager().next_id();

    ctx.registry().action_restrictions[restriction_id] = {
        {top_left, bottom_right}, {action}};
  }
}

void create_road_line(ecs::Context<Registry> &ctx, const std::size_t row_index,
                      const components::Color &color) {
  const float pos_y =
      grid_ticks_to_float(row_index + 1) - ROAD_LINE_WIDTH / 2.0;
  // TODO: randomize initial pos_x value?
  for (float pos_x = -1.06; pos_x < 1.0; pos_x += STEP_SIZE * 0.87) {
    std::vector<glm::vec4> vertices = {
        glm::vec4(pos_x, pos_y, 0.1, 1.0),
        glm::vec4(pos_x + STEP_SIZE * 0.6, pos_y, 0.1, 1.0),
        glm::vec4(pos_x + STEP_SIZE * 0.6, pos_y + ROAD_LINE_WIDTH, 0.1, 1.0),
        glm::vec4(pos_x, pos_y + ROAD_LINE_WIDTH, 0.1, 1.0)};
    ctx.registry().add_render_info(
        ctx, {std::make_unique<components::VertexVector>(std::move(vertices)),
              color, glm::mat4(1)});
  }
}

void create_car(ecs::Context<Registry> &ctx, const float pos_x,
                const std::size_t row_index, const float vel,
                const components::Color &color) {
  const float actual_pos_y = grid_ticks_to_float(row_index) + STEP_SIZE * 0.5f;
  auto vertices = CAR_VERTICES;
  const auto id = ctx.registry().add_render_info(
      ctx,
      {std::make_unique<components::VertexVector>(std::move(vertices)), color,
       glm::translate(glm::mat4(1), glm::vec3(pos_x, actual_pos_y, 0.0f))});
  ctx.registry().cars[id] = {glm::vec3(vel, 0.0, 0.0)};
  create_wheel(ctx, id, glm::vec3(CAR_RADIUS_X / 2, -CAR_RADIUS_Y, 0));
  create_wheel(ctx, id, glm::vec3(-CAR_RADIUS_X / 2, -CAR_RADIUS_Y, 0));
}

void create_wheel(ecs::Context<Registry> &ctx, std::size_t car_id,
                  const glm::vec3 &position) {
  const auto id = ctx.registry().add_render_info(
      ctx, {std::make_unique<components::CircleVertex>(glm::vec4(0, 0, 0.1f, 0),
                                                       WHEEL_RADIUS),
            WHEEL_COLOR, glm::translate(glm::mat4(1), position)});
  ctx.entity_manager().link_parent_child(car_id, id);

  std::vector<glm::vec4> vertices = {
      glm::vec4(WHEEL_RADIUS * 0.8f, -WHEEL_RADIUS * 0.1, 0.15f, 0),
      glm::vec4(WHEEL_RADIUS * 0.8f, WHEEL_RADIUS * 0.1, 0.15f, 0),
      glm::vec4(-WHEEL_RADIUS * 0.8f, WHEEL_RADIUS * 0.1, 0.15f, 0),
      glm::vec4(-WHEEL_RADIUS * 0.8f, -WHEEL_RADIUS * 0.1, 0.15f, 0),
  };
  const auto mark_id = ctx.registry().add_render_info(
      ctx, {std::make_unique<components::VertexVector>(
                std::vector<glm::vec4>(vertices)),
            WHEEL_MARKING_COLOR, glm::mat4(1)});
  ctx.entity_manager().link_parent_child(id, mark_id);
}

void create_map(ecs::Context<Registry> &ctx) {
  fill_map_row(ctx, 0, GRASS_COLOR);
  fill_map_row(ctx, 1, ROAD_COLOR);
  fill_map_row(ctx, 2, ROAD_COLOR);
  fill_map_row(ctx, 3, GRASS_COLOR);
  fill_map_row(ctx, 4, ROAD_COLOR);
  fill_map_row(ctx, 5, ROAD_COLOR);
  fill_map_row(ctx, 6, ROAD_COLOR);
  fill_map_row(ctx, 7, GRASS_COLOR);

  create_tree(ctx, 3, 0, TREE_COLOR);
  create_tree(ctx, 3, 3, TREE_COLOR);
  create_tree(ctx, 3, 6, TREE_COLOR);
  create_tree(ctx, 7, 2, TREE_COLOR);
  create_tree(ctx, 7, 3, TREE_COLOR);
  create_tree(ctx, 7, 5, TREE_COLOR);

  create_road_line(ctx, 1, ROAD_LINE_COLOR);
  create_road_line(ctx, 4, ROAD_LINE_COLOR);
  create_road_line(ctx, 5, ROAD_LINE_COLOR);

  create_car(ctx, -0.7f, 1, 0.2f, CAR_COLOR);
  create_car(ctx, 0.1f, 1, 0.2f, CAR_COLOR);
  create_car(ctx, -0.3f, 2, -0.3f, CAR_COLOR);
  create_car(ctx, 0.1f, 2, -0.3f, CAR_COLOR);
  create_car(ctx, -0.4f, 4, 0.25f, CAR_COLOR);
  create_car(ctx, 0.6f, 4, 0.25f, CAR_COLOR);
  create_car(ctx, -1.0f, 5, 0.35f, CAR_COLOR);
  create_car(ctx, 0.2f, 5, 0.35f, CAR_COLOR);
  create_car(ctx, 0.7f, 5, 0.35f, CAR_COLOR);
  create_car(ctx, -0.8f, 6, -0.15f, CAR_COLOR);
  create_car(ctx, -0.1f, 6, -0.15f, CAR_COLOR);
  create_car(ctx, 0.7f, 6, -0.15f, CAR_COLOR);

  const std::vector<std::pair<BoundingBox, components::ActionKind>>
      adjacent_pos = {{{
                           {1.0f - STEP_SIZE, 1.0f},
                           {1.0f, -1.0f},
                       },
                       components::ActionKind::MOVE_RIGHT},
                      {{{-1.0f, 1.0f}, {-1.0f + STEP_SIZE, -1.0f}},
                       components::ActionKind::MOVE_LEFT},
                      {{{-1.0f, 1.0f}, {1.0f, 1.0f - STEP_SIZE}},
                       components::ActionKind::MOVE_UP},
                      {{{-1.0f, -1.0f + STEP_SIZE}, {1.0f, -1.0f}},
                       components::ActionKind::MOVE_DOWN}};
  for (const auto &p : adjacent_pos) {
    const auto restriction_id = ctx.entity_manager().next_id();
    const auto &bb = p.first;
    const auto &action = p.second;
    ctx.registry().action_restrictions[restriction_id] = {bb, {action}};
  }
}

void create_character(ecs::Context<Registry> &ctx) {
  const auto character_pos = grid_to_world_cell(4, 0).midpoint();
  std::vector<glm::vec4> vertices = {
      glm::vec4(-CHARACTER_RADIUS, -CHARACTER_RADIUS, 0.5f, 1.0f),
      glm::vec4(CHARACTER_RADIUS, -CHARACTER_RADIUS, 0.5f, 1.0f),
      glm::vec4(CHARACTER_RADIUS, CHARACTER_RADIUS, 0.5f, 1.0f),
      glm::vec4(-CHARACTER_RADIUS, CHARACTER_RADIUS, 0.5f, 1.0f)};
  const auto id = ctx.registry().add_render_info(
      ctx, {std::make_unique<components::VertexVector>(std::move(vertices)),
            CHARACTER_COLOR,
            glm::translate(glm::mat4(1), glm::vec3(character_pos[0],
                                                   character_pos[1], 0.0f))});
  ctx.registry().characters[id] = {};
  ctx.registry().animations[id] = {components::AnimationState::BEFORE_START,
                                   {components::AnimationKind::DISABLED, {}},
                                   glm::mat4(1),
                                   0};
  ctx.registry().character_id = id;
}

void create_win_zone(ecs::Context<Registry> &ctx) {
  const auto id = ctx.entity_manager().next_id();
  ctx.registry().win_zones[id] = {
      {glm::vec2(-1, 1), glm::vec2(1, 1 - STEP_SIZE)}};
}
