#pragma once

#include "ecs/systems.hpp"

#include <cstddef>

#include "components.hpp"
#include "registry.hpp"

void fill_map_row(ecs::Context<Registry> &ctx, std::size_t row_index,
                  const components::Color &color);

void create_tree(ecs::Context<Registry> &ctx, std::size_t row_index,
                 std::size_t col_index, const components::Color &color);

void create_road_line(ecs::Context<Registry> &ctx, const std::size_t row_index,
                      const components::Color &color);

void create_car(ecs::Context<Registry> &ctx, const float pos_x,
                const std::size_t row_index, const float vel,
                const components::Color &color);

void create_map(ecs::Context<Registry> &ctx);

void create_character(ecs::Context<Registry> &ctx);

void create_win_zone(ecs::Context<Registry> &ctx);
