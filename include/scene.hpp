#pragma once

#include <ecs/entities.hpp>
#include <ecs/systems.hpp>

#include <cmath>

#include <components.hpp>
#include <grid.hpp>
#include <registry.hpp>

const float PI = std::acos(-1);
const float ROAD_OFFSET = 0.2;
const float CHARACTER_OFFSET = 0.5;
const float TREE_OFFSET = 0.75;

void load_models(ecs::Context<Registry> &ctx);

void setup_camera(ecs::Context<Registry> &ctx);

void create_character(ecs::Context<Registry> &ctx, int col);

void fill_map_row(ecs::Context<Registry> &ctx, std::size_t row_index,
                  TileType tile_type);

void create_map(ecs::Context<Registry> &ctx);
