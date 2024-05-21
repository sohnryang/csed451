#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <cstddef>
#include <queue>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "components.hpp"
#include "model.hpp"
#include "shader_program.hpp"
#include "texture.hpp"

enum class GameState { IN_PROGRESS, LOSE, WIN };

enum class InputKind { UP, DOWN, LEFT, RIGHT };

enum class TileType { ROAD, GRASS };

struct Registry {
  std::unordered_map<ecs::entities::EntityId, components::Mesh> meshes;
  std::unordered_map<ecs::entities::EntityId, components::Character> characters;
  std::unordered_map<ecs::entities::EntityId, components::ActionRestriction>
      action_restrictions;
  std::unordered_map<ecs::entities::EntityId, components::Car> cars;
  std::unordered_map<ecs::entities::EntityId, components::WinZone> win_zones;
  std::unordered_map<ecs::entities::EntityId, components::Animation> animations;
  std::unordered_map<ecs::entities::EntityId, components::ShoeItem> shoe_items;
  std::unordered_set<ecs::entities::EntityId> wheels;
  std::unordered_set<ecs::entities::EntityId> truck_plates;

  GameState state = GameState::IN_PROGRESS;
  ecs::entities::EntityId character_id;
  std::queue<InputKind> input_queue;
  std::unordered_set<components::ActionKind> blocked_actions;
  bool pass_through = false;
  bool diffuse_on = true;
  bool normal_mapping_on = true;
  int view_mode = 0;
  std::vector<components::CameraConfig> camera_config;
  glm::vec3 camera_init;
  components::LightConfig light_config = {
      glm::vec3(), glm::vec3(), 0, 3, 1, 4, 1,
  };
  float directional_light_angle = 0.0f;
  const std::vector<std::string> model_filenames = {
      "rooster.obj",  "tree.obj",  "car.obj",   "truck.obj",
      "sneakers.obj", "floor.obj", "floor2.obj"};
  std::unordered_map<std::string, std::size_t> model_indices;
  std::vector<Model> models;
  const std::vector<std::string> texture_filenames = {
      "empty_texture.png", "rooster_texture.jpg", "tree_texture.png",
      "car_texture.png",   "truck_texture.jpg",   "ground_texture.jpg",
      "road_texture.jpg"};
  std::unordered_map<std::string, std::size_t> texture_indicies;
  std::vector<Texture> textures;

  static constexpr std::size_t GOURAUD_SHADER = 0, PHONG_SHADER = 1;
  std::vector<ShaderProgram> shader_programs;
  std::size_t program_index = GOURAUD_SHADER;

  std::size_t player_row = 0;
  std::size_t score = 0;
  std::size_t map_top_generated = 1;
  bool map_generate_finished = false;
  bool item_placed = false;
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
      std::uniform_real_distribution<double>(-3, 3);
  std::uniform_real_distribution<double> random_probability_dist =
      std::uniform_real_distribution<double>(0.0, 1.0);

  Registry();

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
