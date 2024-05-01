#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <cmath>

#include "components.hpp"
#include "grid.hpp"
#include "registry.hpp"

const float PI = std::acos(-1);
const float ROAD_OFFSET = 0.2;
const float SHOE_OFFSET = 0.3;
const float CAR_SPAWN_DENSITY = 0.4f;
const float TRUCK_RATE = 0.3f;

void setup_camera(ecs::Context<Registry> &ctx, int col);

void create_character(ecs::Context<Registry> &ctx, int col);

void fill_map_row(ecs::Context<Registry> &ctx, int row_index,
                  TileType tile_type);

void create_map(ecs::Context<Registry> &ctx);

void create_car(ecs::Context<Registry> &ctx, const float pos_x,
                const std::size_t row_index, const float vel);

void create_truck(ecs::Context<Registry> &ctx, const float pos_x,
                  const std::size_t row_index, const float vel);
