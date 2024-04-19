#include <scene.hpp>

#include <ecs/systems.hpp>

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <cstddef>
#include <iostream>
#include <stdexcept>

#include <components.hpp>
#include <grid.hpp>
#include <registry.hpp>
#include <systems.hpp>

void load_models(ecs::Context<Registry> &ctx) {
  for (const auto &filename : ctx.registry().model_filenames) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./";
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filename, reader_config))
      throw std::runtime_error("obj file parse failed");

    if (!reader.Warning().empty())
      std::cout << "TinyObjReader: " << reader.Warning();

    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();
    const auto &materials = reader.GetMaterials();
    for (std::size_t s = 0; s < shapes.size(); s++) {
      std::size_t index_offset = 0;
      for (std::size_t f = 0; f < shapes[s].mesh.num_face_vertices.size();
           f++) {
        const auto fv = shapes[s].mesh.num_face_vertices[f];
        for (std::size_t v = 0; v < fv; v++) {
          const auto idx = shapes[s].mesh.indices[index_offset + v];
          const auto vx = attrib.vertices[3 * idx.vertex_index],
                     vy = attrib.vertices[3 * idx.vertex_index + 1],
                     vz = attrib.vertices[3 * idx.vertex_index + 2];
          ctx.registry().model_vertices[filename].push_back(
              glm::vec3(vx, vy, vz));
        }
        index_offset += fv;
      }
    }

    std::cout << "Loaded obj file: " << filename << std::endl;
  }
}

void setup_camera(ecs::Context<Registry> &ctx) {
  ctx.registry().camera_config = {glm::vec3(10, 10, 10),
                                  glm::vec3(0, 0, 0),
                                  glm::vec3(0, 1, 0),
                                  40,
                                  1,
                                  0.1,
                                  100};
}

void create_character(ecs::Context<Registry> &ctx, int col) {
  printf("%d\n", col);
  const auto character_vertices = ctx.registry().model_vertices["rooster.obj"];
  const auto character_pos = grid_to_world_cell(col, 0).midpoint()[0];
  const auto id = ctx.registry().add_mesh(
      ctx, {character_vertices,
            glm::translate(glm::mat4(1), glm::vec3(character_pos, CHARACTER_OFFSET, 0))});
  ctx.registry().character_id = id;
  ctx.registry().animations[id] = {components::AnimationState::BEFORE_START,
                                   {components::AnimationKind::DISABLED, {}},
                                   glm::mat4(1),
                                   0};
}

void fill_map_row(ecs::Context<Registry> &ctx, std::size_t row_index, TileType tile_type) {
  const auto vertices = ctx.registry().model_vertices["floor.obj"];
  float delta_y = -1.0;
  if (tile_type == TileType::ROAD)
    delta_y -= ROAD_OFFSET;
  ctx.registry().add_mesh(
      ctx, {vertices,
            glm::translate(glm::mat4(1), glm::vec3(0, delta_y, (int)row_index * -2))});
}

void create_tree(ecs::Context<Registry> &ctx, std::size_t row_index,
                 std::size_t col_index) {
  // might move this constant (0.75) to somewhere
  const auto tree_pos = grid_to_world_cell(col_index, row_index).midpoint();
  const auto vertices = ctx.registry().model_vertices["tree.obj"];
  const auto id = ctx.registry().add_mesh(
      ctx, {vertices,
            glm::translate(glm::mat4(1),
                           glm::vec3(tree_pos[0], TREE_OFFSET, -tree_pos[1]))});

  /*
  const std::vector<std::pair<glm::vec2, components::ActionKind>> adjacent_pos =
      {{{-1, 0}, components::ActionKind::MOVE_RIGHT},
       {{1, 0}, components::ActionKind::MOVE_LEFT},
       {{0, -1}, components::ActionKind::MOVE_FORWARD},
       {{0, 1}, components::ActionKind::MOVE_BACK}};
  for (const auto &p : adjacent_pos) {
    const auto delta = p.first;
    const auto action = p.second;
    const auto rect_center = STEP_SIZE * delta + tree_pos;
    const auto diagonal = glm::vec2(TREE_RADIUS, -TREE_RADIUS);
    const auto top_left = rect_center - diagonal;
    const auto bottom_right = rect_center + diagonal;
    const auto restriction_id = ctx.entity_manager().next_id();

    ctx.registry().action_restrictions[restriction_id] = {
        {top_left, bottom_right}, {action}, false};
  }
  */
}

/*

void create_car(ecs::Context<Registry> &ctx, const float pos_x,
                const std::size_t row_index, const float vel,
                const components::Color &color) {
  const float actual_pos_y = grid_ticks_to_float(row_index) + STEP_SIZE * 0.5f;
  auto vertices = CAR_VERTICES;
  auto translate_mat =
      glm::translate(glm::mat4(1), glm::vec3(pos_x, actual_pos_y, 0.0f));
  if (vel <= 0.0f)
    translate_mat =
        glm::scale(glm::mat4(1), glm::vec3(-1.0f, 1.0f, 1.0f)) * translate_mat;
  const auto id = ctx.registry().add_render_info(
      ctx, {std::make_unique<components::VertexVector>(std::move(vertices)),
            color, translate_mat});
  ctx.registry().cars[id] = {glm::vec3(vel, 0.0, 0.0)};
  create_wheel(ctx, id, glm::vec3(CAR_RADIUS_X / 2, -CAR_RADIUS_Y, 0));
  create_wheel(ctx, id, glm::vec3(-CAR_RADIUS_X / 2, -CAR_RADIUS_Y, 0));
}

void create_truck(ecs::Context<Registry> &ctx, const float pos_x,
                  const std::size_t row_index, const float vel,
                  const components::Color &color) {
  const float actual_pos_y = grid_ticks_to_float(row_index) + STEP_SIZE * 0.5f;
  auto vertices = TRUCK_VERTICES;
  auto translate_mat =
      glm::translate(glm::mat4(1), glm::vec3(pos_x, actual_pos_y, 0.0f));
  if (vel <= 0.0f)
    translate_mat =
        glm::scale(glm::mat4(1), glm::vec3(-1.0f, 1.0f, 1.0f)) * translate_mat;
  const auto id = ctx.registry().add_render_info(
      ctx, {std::make_unique<components::VertexVector>(std::move(vertices)),
            color, translate_mat});
  ctx.registry().cars[id] = {glm::vec3(vel, 0.0, 0.0)};
  create_wheel(ctx, id, glm::vec3(CAR_RADIUS_X / 2, -CAR_RADIUS_Y, 0));
  create_wheel(ctx, id, glm::vec3(-CAR_RADIUS_X / 2, -CAR_RADIUS_Y, 0));
  create_plate(ctx, id, glm::vec3(CAR_RADIUS_X_SHORT, 0, 0));
}

void create_wheel(ecs::Context<Registry> &ctx, std::size_t car_id,
                  const glm::vec3 &position) {
  const auto id = ctx.registry().add_render_info(
      ctx, {std::make_unique<components::CircleVertex>(glm::vec4(0, 0, 0.1f, 0),
                                                       WHEEL_RADIUS),
            WHEEL_COLOR, glm::translate(glm::mat4(1), position)});
  ctx.entity_manager().link_parent_child(car_id, id);
  ctx.registry().wheels.insert(id);

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

void create_plate(ecs::Context<Registry> &ctx, std::size_t car_id,
                  const glm::vec3 &position) {
  auto vertices = TRUCK_PLATE_VERTICES;
  const auto id = ctx.registry().add_render_info(
      ctx, {std::make_unique<components::VertexVector>(std::move(vertices)),
            TRUCK_PLATE_COLOR, glm::translate(glm::mat4(1), position)});
  systems::Animation::set(ctx, id,
                          {components::AnimationKind::LOOP,
                           {{0.0f, glm::mat4(1)},
                            {float(components::Car::TRUCK_PLATE_DURATION),
                             glm::rotate(glm::mat4(1), glm::radians(-20.0f),
                                         glm::vec3(0, 0, 1))}}});
  ctx.entity_manager().link_parent_child(car_id, id);
  ctx.registry().truck_plates.insert(id);
}

void create_shoe_item(ecs::Context<Registry> &ctx, std::size_t row_index,
                      std::size_t col_index) {
  const auto position = grid_to_world_cell(col_index, row_index).midpoint();
  auto vertices = SHOE_VERTICES;
  const auto shoe_id = ctx.registry().add_render_info(
      ctx,
      {std::make_unique<components::VertexVector>(std::move(vertices)),
       SHOE_COLOR,
       glm::translate(glm::mat4(1), glm::vec3(position[0], position[1], 0))});
  ctx.registry().shoe_items[shoe_id] = {};
}

*/

bool check_map_valid(std::vector<std::vector<bool>> check, int start_col) {
  // Intended not to use ref for 2D vector, make a copy for here
  constexpr int dx[] = {0, -1, 0, 1}, dy[] = {1, 0, -1, 0};
  std::queue<std::pair<int, int>> q;
  q.push({0, start_col});
  check[0][start_col] = true;
  while (!q.empty()) {
    auto cur = q.front();
    q.pop();
    if (cur.first == 7)
      return true;
    for (int i = 0; i < 4; i++) {
      int nx = cur.first + dx[i], ny = cur.second + dy[i];
      if (0 <= nx && nx < 8 && 0 <= ny && ny < 8 && !check[nx][ny]) {
        check[nx][ny] = true;
        q.push({nx, ny});
      }
    }
  }
  return false;
}

void create_map(ecs::Context<Registry> &ctx) {

  /*fill_map_row(ctx, 0, GRASS_COLOR);
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
  create_truck(ctx, -0.3f, 2, -0.3f, TRUCK_COLOR);
  create_car(ctx, 0.4f, 2, -0.3f, CAR_COLOR);
  create_car(ctx, -0.4f, 4, 0.25f, CAR_COLOR);
  create_car(ctx, 0.6f, 4, 0.25f, CAR_COLOR);
  create_truck(ctx, -1.0f, 5, 0.35f, TRUCK_COLOR);
  create_car(ctx, 0.2f, 5, 0.35f, CAR_COLOR);
  create_car(ctx, 0.7f, 5, 0.35f, CAR_COLOR);
  create_truck(ctx, -0.8f, 6, -0.15f, TRUCK_COLOR);
  create_car(ctx, -0.1f, 6, -0.15f, CAR_COLOR);
  create_car(ctx, 0.7f, 6, -0.15f, CAR_COLOR);

  create_shoe_item(ctx, 3, 2);*/

  // Set each row tile type
  std::vector<TileType> map_data(8, TileType::GRASS);
  while (true) {
    int grass_count = 0, road_count = 0;
    for (int i = 1; i <= 6; i++) {
      map_data[i] = ctx.registry().random_tile_type(ctx);
      if (map_data[i] == TileType::GRASS)
        grass_count++;
      else if (map_data[i] == TileType::ROAD)
        road_count++;
    }
    if (grass_count >= 1 && road_count >= 4)
      break;
  }
  for (int i = 0; i < 8; i++)
    fill_map_row(ctx, i, map_data[i]);

  // Set tree position
  std::vector<std::vector<bool>> tree_pos(8, std::vector<bool>(8, false));
  int start_col = 0;
  while (true) {
    for (int i = 0; i < 8; i++)
      std::fill(tree_pos[i].begin(), tree_pos[i].end(), false);
    for (int i = 0; i < 8; i++) {
      if (map_data[i] == TileType::ROAD)
        continue;
      int tree_count = ctx.registry().random_tree_number(ctx);
      for (int j = 0; j < tree_count; j++) {
        int col = ctx.registry().random_column(ctx, tree_pos[i]);
        tree_pos[i][col] = true;
      }
      if (i == 0) {
        int col = ctx.registry().random_column(ctx, tree_pos[i]);
        start_col = col;
      }
    }
    if (check_map_valid(tree_pos, start_col))
      break;
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (tree_pos[i][j])
        create_tree(ctx, i, j);
    }
  }

  /*

  // Set cars on the road
  for (int i = 0; i < 8; i++) {
    if (map_data[i] != TileType::ROAD)
      continue;
    auto vel = ctx.registry().random_speed(ctx);
    // Generate car with 40% density
    // 70% car, 30% truck
    for (double j = 0.0; j <= 2.0; j += 0.4) {
      if (ctx.registry().random_probability(ctx, CAR_SPAWN_DENSITY)) {
        if (ctx.registry().random_probability(ctx, TRUCK_RATE))
          create_truck(ctx, j, i, vel, TRUCK_COLOR);
        else
          create_car(ctx, j, i, vel, CAR_COLOR);
      }
    }
  }

  // Set map bound
  const std::vector<std::pair<BoundingBox, components::ActionKind>>
      adjacent_pos = {{{{1.0f - STEP_SIZE * 0.75f, 1.0f}, {1.0f, -1.0f}},
                       components::ActionKind::MOVE_RIGHT},
                      {{{-1.0f, 1.0f}, {-1.0f + STEP_SIZE * 0.75f, -1.0f}},
                       components::ActionKind::MOVE_LEFT},
                      {{{-1.0f, 1.0f}, {1.0f, 1.0f - STEP_SIZE * 0.75f}},
                       components::ActionKind::MOVE_FORWARD},
                      {{{-1.0f, -1.0f + STEP_SIZE * 0.75f}, {1.0f, -1.0f}},
                       components::ActionKind::MOVE_BACK}};
  for (const auto &p : adjacent_pos) {
    const auto restriction_id = ctx.entity_manager().next_id();
    const auto &bb = p.first;
    const auto &action = p.second;
    ctx.registry().action_restrictions[restriction_id] = {bb, {action}, true};
  }

  // Set item
  std::vector<int> row_idx(7);
  tree_pos[0][start_col] = true;
  for (int i = 0; i < 7; i++)
    row_idx[i] = i;
  std::shuffle(row_idx.begin(), row_idx.end(), ctx.random_gen());
  for (int row : row_idx) {
    if (map_data[row] == TileType::ROAD)
      continue;
    int col = ctx.registry().random_column(ctx, tree_pos[row]);
    create_shoe_item(ctx, row, col);
    break;
  }

  */

  // Set character
  create_character(ctx, start_col);
}

