#include "scene.hpp"

#include "ecs/systems.hpp"

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include <cstddef>

#include "bounding_box.hpp"
#include "components.hpp"
#include "grid.hpp"
#include "registry.hpp"

void setup_camera(ecs::Context<Registry> &ctx, int col) {
  const auto character_pos = grid_to_world(0, col, 0, col).midpoint()[0];
  ctx.registry().camera_config.push_back(components::CameraConfig(
      {glm::vec3(character_pos, 2, 5), glm::vec3(character_pos, 0, -10),
       glm::vec3(0, 1, 0), 40, 1, 0.1, 50}));
  ctx.registry().camera_config.push_back(components::CameraConfig(
      {glm::vec3(character_pos, 0.5, -1), glm::vec3(character_pos, 0, -100),
       glm::vec3(0, 1, 0), 60, 1, 0.1, 50}));
  glm::vec3 camera_pos = {4, 10, 10}, midpoint = {0, 0, -STEP_SIZE};
  ctx.registry().camera_config.push_back(components::CameraConfig(
      {midpoint + camera_pos, midpoint, glm::vec3(0, 1, 0), 60, 1, 0.1, 100}));
  /*
  ctx.registry().camera_config.push_back(components::CameraConfig(
      {midpoint + glm::vec3(0, 60, 5), midpoint, glm::vec3(0, 1, 0), 55, 1, 0.1,
  100}));
      */
}

void create_character(ecs::Context<Registry> &ctx, int col) {
  const auto character_pos = grid_to_world(0, col, 0, col).midpoint()[0];
  const auto model_index = ctx.registry().model_indices["rooster.obj"];
  const auto texture_index =
      ctx.registry().texture_indicies["rooster_texture.jpg"];
  const auto id = ctx.registry().add_mesh(
      ctx, {
               model_index,
               texture_index,
               glm::translate(glm::mat4(1), glm::vec3(character_pos, 0, 0)),
           });
  ctx.registry().camera_init = glm::vec3(character_pos, 0, 0);
  ctx.registry().character_id = id;
  ctx.registry().animations[id] = {
      components::AnimationState::BEFORE_START,
      {
          components::AnimationKind::DISABLED,
          {},
      },
      glm::mat4(1),
      0,
  };
  auto &character = ctx.registry().characters[id];
  auto &mesh = ctx.registry().meshes[id];
  character.model_bb = ctx.registry().models[model_index].bounding_box;
}

void fill_map_row(ecs::Context<Registry> &ctx, int row_index,
                  TileType tile_type) {
  float delta_y = 0.0;
  if (tile_type == TileType::ROAD)
    delta_y -= ROAD_OFFSET;
  const auto texture_index =
      ctx.registry().texture_indicies["empty_texture.png"];
  ctx.registry().add_mesh(
      ctx,
      {ctx.registry().model_indices["floor2.obj"], texture_index,
       glm::translate(glm::mat4(1),
                      glm::vec3(0, delta_y, (int)row_index * -STEP_SIZE))});
}

void create_tree(ecs::Context<Registry> &ctx, std::size_t row_index,
                 std::size_t col_index) {
  // might move this constant (0.75) to somewhere
  const auto tree_pos =
      grid_to_world(row_index, col_index, row_index, col_index).midpoint();
  const auto texture_index =
      ctx.registry().texture_indicies["tree_texture.png"];
  const auto id = ctx.registry().add_mesh(
      ctx,
      {ctx.registry().model_indices["tree.obj"], texture_index,
       glm::translate(glm::mat4(1), glm::vec3(tree_pos[0], 0, tree_pos[2]))});

  const std::vector<std::pair<BoundingBox3D, components::ActionKind>>
      adjacent_pos = {
          {grid_to_world(row_index, col_index - 1, row_index, col_index - 1),
           components::ActionKind::MOVE_RIGHT},
          {grid_to_world(row_index, col_index + 1, row_index, col_index + 1),
           components::ActionKind::MOVE_LEFT},
          {grid_to_world(row_index + 1, col_index, row_index + 1, col_index),
           components::ActionKind::MOVE_BACK},
          {grid_to_world(row_index - 1, col_index, row_index - 1, col_index),
           components::ActionKind::MOVE_FORWARD},
      };
  for (const auto &p : adjacent_pos) {
    const auto restriction_id = ctx.entity_manager().next_id();
    const auto &bb = p.first;
    const auto &action = p.second;
    ctx.registry().action_restrictions[restriction_id] = {bb, {action}, false};
  }
}

void create_car(ecs::Context<Registry> &ctx, const float pos_x,
                const std::size_t row_index, const float vel) {
  const float actual_pos_z = -STEP_SIZE * row_index;
  auto translate_mat = glm::translate(
      glm::mat4(1), glm::vec3(pos_x, -ROAD_OFFSET, actual_pos_z));
  if (vel <= 0.0f)
    translate_mat =
        glm::scale(glm::mat4(1), glm::vec3(-1.0f, 1.0f, 1.0f)) * translate_mat;
  const auto model_index = ctx.registry().model_indices["car.obj"];
  const auto texture_index = ctx.registry().texture_indicies["car_texture.png"];
  const auto id =
      ctx.registry().add_mesh(ctx, {model_index, texture_index, translate_mat});
  const auto &mesh = ctx.registry().meshes[id];
  ctx.registry().cars[id] = {glm::vec3(vel, 0.0, 0.0),
                             ctx.registry().models[model_index].bounding_box};
}

void create_truck(ecs::Context<Registry> &ctx, const float pos_x,
                  const std::size_t row_index, const float vel) {
  const float actual_pos_z = -STEP_SIZE * row_index;
  auto translate_mat = glm::translate(
      glm::mat4(1), glm::vec3(pos_x, -ROAD_OFFSET, actual_pos_z));
  if (vel <= 0.0f)
    translate_mat =
        glm::scale(glm::mat4(1), glm::vec3(-1.0f, 1.0f, 1.0f)) * translate_mat;
  const auto model_index = ctx.registry().model_indices["truck.obj"];
  const auto texture_index =
      ctx.registry().texture_indicies["truck_texture.jpg"];
  const auto id =
      ctx.registry().add_mesh(ctx, {model_index, texture_index, translate_mat});
  ctx.registry().cars[id] = {glm::vec3(vel, 0.0, 0.0)};
  const auto &mesh = ctx.registry().meshes[id];
  ctx.registry().cars[id] = {glm::vec3(vel, 0.0, 0.0),
                             ctx.registry().models[model_index].bounding_box};
}

void create_shoe_item(ecs::Context<Registry> &ctx, std::size_t row_index,
                      std::size_t col_index) {
  const auto position = glm::vec3(col_index * STEP_SIZE - 3.5f * STEP_SIZE,
                                  SHOE_OFFSET, -STEP_SIZE * row_index);
  const auto model_index = ctx.registry().model_indices["sneakers.obj"];
  const auto texture_index =
      ctx.registry().texture_indicies["empty_texture.png"];
  const auto shoe_id =
      ctx.registry().add_mesh(ctx, {model_index, texture_index,
                                    glm::translate(glm::mat4(1), position)});
  ctx.registry().shoe_items[shoe_id] = {
      ctx.registry().models[model_index].bounding_box};
}

bool check_map_valid(std::vector<std::vector<bool>> check) {
  // Intended not to use ref for 2D vector, make a copy for here
  constexpr int dx[] = {0, -1, 0, 1}, dy[] = {1, 0, -1, 0};
  std::queue<std::pair<int, int>> q;
  for (int i = 0; i < 8; i++) {
    q.push({0, i});
    check[0][i] = true;
  }
  while (!q.empty()) {
    auto cur = q.front();
    q.pop();
    if (cur.first == check.size() - 1)
      return true;
    for (int i = 0; i < 4; i++) {
      int nx = cur.first + dx[i], ny = cur.second + dy[i];
      if (0 <= nx && nx < check.size() && 0 <= ny && ny < GRID_SIZE &&
          !check[nx][ny]) {
        check[nx][ny] = true;
        q.push({nx, ny});
      }
    }
  }
  return false;
}

void create_map(ecs::Context<Registry> &ctx) {

  int grass_length = ctx.registry().random_tile_length(ctx);
  for (int i = 0; i < grass_length; i++)
    fill_map_row(ctx, ctx.registry().map_top_generated + i, TileType::GRASS);

  // Set tree position
  std::vector<std::vector<bool>> tree_pos(grass_length + 2,
                                          std::vector<bool>(GRID_SIZE, false));
  int start_col = 0;
  while (true) {
    for (int i = 1; i <= grass_length; i++)
      std::fill(tree_pos[i].begin(), tree_pos[i].end(), false);
    for (int i = 1; i <= grass_length; i++) {
      int tree_count = ctx.registry().random_tree_number(ctx);
      for (int j = 0; j < tree_count; j++) {
        int col = ctx.registry().random_column(ctx, tree_pos[i]);
        tree_pos[i][col] = true;
      }
    }
    if (check_map_valid(tree_pos))
      break;
  }
  for (int i = 0; i < grass_length; i++) {
    for (int j = 0; j < 8; j++) {
      if (tree_pos[i + 1][j])
        create_tree(ctx, ctx.registry().map_top_generated + i, j);
    }
  }

  // Set item
  if (!ctx.registry().item_placed) {
    ctx.registry().item_placed = true;
    std::vector<int> row_idx(grass_length);
    for (int i = 0; i < grass_length; i++)
      row_idx[i] = i;
    std::shuffle(row_idx.begin(), row_idx.end(), ctx.random_gen());
    for (int row : row_idx) {
      int col = ctx.registry().random_column(ctx, tree_pos[row + 1]);
      create_shoe_item(ctx, ctx.registry().map_top_generated + row, col);
      break;
    }
  }

  ctx.registry().map_top_generated += grass_length;

  int road_length = ctx.registry().random_tile_length(ctx);
  for (int i = 0; i < road_length; i++)
    fill_map_row(ctx, ctx.registry().map_top_generated + i, TileType::ROAD);

  // Set cars on the road
  for (int i = 0; i < road_length; i++) {
    auto vel = ctx.registry().random_speed(ctx);
    // Generate car with 40% density
    // 70% car, 30% truck
    for (double j = (-2.5 * GRID_SIZE) * STEP_SIZE; j <= (1.5 * GRID_SIZE);
         j += 3.0) {
      if (ctx.registry().random_probability(ctx, CAR_SPAWN_DENSITY)) {
        if (ctx.registry().random_probability(ctx, TRUCK_RATE))
          create_truck(ctx, j, ctx.registry().map_top_generated + i, vel);
        else
          create_car(ctx, j, ctx.registry().map_top_generated + i, vel);
      }
    }
  }

  ctx.registry().map_top_generated += road_length;

  // Set map bound
  const std::vector<std::pair<BoundingBox3D, components::ActionKind>>
      adjacent_pos = {
          {grid_to_world(
               ctx.registry().map_top_generated - grass_length - road_length,
               GRID_SIZE - 1, ctx.registry().map_top_generated, GRID_SIZE - 1),
           components::ActionKind::MOVE_RIGHT},
          {grid_to_world(ctx.registry().map_top_generated - grass_length -
                             road_length,
                         0, ctx.registry().map_top_generated, 0),
           components::ActionKind::MOVE_LEFT},
      };
  for (const auto &p : adjacent_pos) {
    const auto restriction_id = ctx.entity_manager().next_id();
    const auto &bb = p.first;
    const auto &action = p.second;
    ctx.registry().action_restrictions[restriction_id] = {bb, {action}, true};
  }
}

void create_map_init(ecs::Context<Registry> &ctx) {
  for (int i = -4; i <= 0; i++)
    fill_map_row(ctx, ctx.registry().map_top_generated + i, TileType::GRASS);

  int start_col = ctx.registry().random_column(ctx);

  // Set character
  create_character(ctx, start_col);

  // Set camera
  setup_camera(ctx, start_col);

  const std::vector<std::pair<BoundingBox3D, components::ActionKind>>
      adjacent_pos = {
          {grid_to_world(0, GRID_SIZE - 1, 0, GRID_SIZE - 1),
           components::ActionKind::MOVE_RIGHT},
          {grid_to_world(0, 0, 0, 0), components::ActionKind::MOVE_LEFT},
          {grid_to_world(0, 0, 0, GRID_SIZE - 1),
           components::ActionKind::MOVE_BACK},
      };
  for (const auto &p : adjacent_pos) {
    const auto restriction_id = ctx.entity_manager().next_id();
    const auto &bb = p.first;
    const auto &action = p.second;
    ctx.registry().action_restrictions[restriction_id] = {bb, {action}, true};
  }
}

void create_map_finish(ecs::Context<Registry> &ctx) {
  auto top = ctx.registry().map_top_generated;
  for (int i = 0; i < 32; i++)
    fill_map_row(ctx, top + i, TileType::GRASS);
  const auto win_zone_id = ctx.entity_manager().next_id();
  ctx.registry().win_zones[win_zone_id] = {
      grid_to_world(top, 0, top, GRID_SIZE - 1)};
}
