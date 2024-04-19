#pragma once

#include <ecs/entities.hpp>
#include <ecs/systems.hpp>

#include <queue>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include <components.hpp>

enum class GameState { IN_PROGRESS, LOSE, WIN };

enum class InputKind { UP, DOWN, LEFT, RIGHT };

enum class TileType { ROAD, GRASS };

struct Registry {
  std::unordered_map<ecs::entities::EntityId, components::Mesh> meshes;
  std::unordered_map<ecs::entities::EntityId, components::Character> characters;
  std::unordered_map<ecs::entities::EntityId, components::ActionRestriction>
      action_restrictions;
  std::unordered_map<ecs::entities::EntityId, components::Car> cars;
  GameState state = GameState::IN_PROGRESS;
  std::unordered_map<ecs::entities::EntityId, components::WinZone> win_zones;
  std::unordered_map<ecs::entities::EntityId, components::Animation> animations;
  std::unordered_map<ecs::entities::EntityId, components::ShoeItem> shoe_items;
  std::unordered_set<ecs::entities::EntityId> wheels;
  std::unordered_set<ecs::entities::EntityId> truck_plates;

  ecs::entities::EntityId character_id;
  std::queue<InputKind> input_queue;
  std::unordered_set<components::ActionKind> blocked_actions;
  bool pass_through = false;
  bool hidden_line_removal = false;
  int view_mode = 0;
  std::vector<components::CameraConfig> camera_config;
  glm::vec3 camera_init;
  std::unordered_map<std::string, std::vector<glm::vec3>> model_vertices;
  const std::vector<std::string> model_filenames = {
      "rooster.obj", "tree.obj",     "car.obj",
      "truck.obj",   "sneakers.obj", "floor.obj"};


  size_t map_bottom = 0;
  size_t map_top_generated = 0;
  TileType last_generated = TileType::GRASS;
  std::uniform_int_distribution<int> random_tile_type_dist =
      std::uniform_int_distribution<int>(0, 1);
  std::uniform_int_distribution<int> random_tile_length_dist =
      std::uniform_int_distribution<int>(1, 3);
  std::uniform_int_distribution<int> random_column_dist =
      std::uniform_int_distribution<int>(0, 7);
  std::uniform_int_distribution<int> random_tree_number_dist =
      std::uniform_int_distribution<int>(1, 3);
  std::uniform_real_distribution<double> random_speed_dist =
      std::uniform_real_distribution<double>(-1, 1);
  std::uniform_real_distribution<double> random_probability_dist =
      std::uniform_real_distribution<double>(0.0, 1.0);

  ecs::entities::EntityId add_mesh(ecs::Context<Registry> &ctx,
                                   components::Mesh &&mesh);
  TileType random_tile_type(ecs::Context<Registry> &ctx);
  int random_tile_length(ecs::Context<Registry> &ctx);
  int random_column(ecs::Context<Registry> &ctx);
  int random_column(ecs::Context<Registry> &ctx, std::vector<bool> &ref);
  int random_tree_number(ecs::Context<Registry> &ctx);
  double random_speed(ecs::Context<Registry> &ctx);
  bool random_probability(ecs::Context<Registry> &ctx, double p);
};
