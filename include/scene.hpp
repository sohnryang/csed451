#pragma once

#include "ecs/systems.hpp"

#include <cstddef>

#include "components.hpp"
#include "grid.hpp"
#include "registry.hpp"

const float TREE_RADIUS = STEP_SIZE * 0.75f / 2.0f,
            ROAD_LINE_WIDTH = STEP_SIZE * 0.05f,
            CAR_RADIUS_X = STEP_SIZE * 1.5f / 2.0f,
            CAR_RADIUS_X_SHORT = STEP_SIZE * 0.7f / 2.0f,
            CAR_RADIUS_Y = STEP_SIZE * 0.7f / 2.0f,
            WHEEL_RADIUS = STEP_SIZE * 0.125f, CHARACTER_RADIUS = 0.1f,
            SHOE_RADIUS = STEP_SIZE * 0.375f;

const components::Color GRASS_COLOR = {68.0 / 255, 132.0 / 255, 46.0 / 255},
                        ROAD_COLOR = {172.0 / 255, 172.0 / 255, 172.0 / 255},
                        TREE_COLOR = {200.0 / 255, 131.0 / 255, 0.0 / 255},
                        ROAD_LINE_COLOR = {255.0 / 255, 255.0 / 255,
                                           255.0 / 255},
                        CAR_COLOR = {66.0 / 255, 147.0 / 255, 252.0 / 255},
                        WHEEL_COLOR = {0, 0, 0},
                        WHEEL_MARKING_COLOR = {1, 1, 1},
                        CHARACTER_COLOR = {1, 1, 1},
                        SHOE_COLOR = {0, 170.0f / 255, 1};

const std::vector<glm::vec4>
    CAR_VERTICES = {glm::vec4(-CAR_RADIUS_X, -CAR_RADIUS_Y, 0.5f, 1.0f),
                    glm::vec4(CAR_RADIUS_X, -CAR_RADIUS_Y, 0.5f, 1.0f),
                    glm::vec4(CAR_RADIUS_X, 0.0f, 0.5f, 1.0f),
                    glm::vec4(CAR_RADIUS_X_SHORT, 0.0f, 0.5f, 1.0f),
                    glm::vec4(CAR_RADIUS_X_SHORT, CAR_RADIUS_Y, 0.5f, 1.0f),
                    glm::vec4(-CAR_RADIUS_X_SHORT, CAR_RADIUS_Y, 0.5f, 1.0f),
                    // This coord works abnormal, idk why
                    glm::vec4(-CAR_RADIUS_X_SHORT, 0.0f, 0.5f, 1.0f),
                    glm::vec4(-CAR_RADIUS_X, 0.0f, 0.5f, 1.0f)},
    TRUCK_VERTICES = {glm::vec4(-CAR_RADIUS_X, -CAR_RADIUS_Y, 0.5f, 1.0f),
                      glm::vec4(CAR_RADIUS_X, -CAR_RADIUS_Y, 0.5f, 1.0f),
                      glm::vec4(CAR_RADIUS_X, CAR_RADIUS_Y, 0.5f, 1.0f),
                      glm::vec4(CAR_RADIUS_X_SHORT, CAR_RADIUS_Y, 0.5f, 1.0f),
                      glm::vec4(CAR_RADIUS_X_SHORT, 0.0f, 0.5f, 1.0f),
                      glm::vec4(-CAR_RADIUS_X, 0.0f, 0.5f, 1.0f)},
    SHOE_VERTICES = {glm::vec4(0.0f, 0.0f, 0.5f, 1.0f),
                     glm::vec4(0.0f, SHOE_RADIUS, 0.5f, 1.0f),
                     glm::vec4(-SHOE_RADIUS, SHOE_RADIUS, 0.5f, 1.0f),
                     glm::vec4(-SHOE_RADIUS, -SHOE_RADIUS, 0.5f, 1.0f),
                     glm::vec4(SHOE_RADIUS, -SHOE_RADIUS, 0.5f, 1.0f),
                     glm::vec4(SHOE_RADIUS, 0, 0.5f, 1.0f)};

void fill_map_row(ecs::Context<Registry> &ctx, std::size_t row_index,
                  const components::Color &color);

void create_tree(ecs::Context<Registry> &ctx, std::size_t row_index,
                 std::size_t col_index, const components::Color &color);

void create_road_line(ecs::Context<Registry> &ctx, const std::size_t row_index,
                      const components::Color &color);

void create_truck(ecs::Context<Registry> &ctx, const float pos_x,
                  const std::size_t row_index, const float vel,
                  const components::Color &color);

void create_car(ecs::Context<Registry> &ctx, const float pos_x,
                const std::size_t row_index, const float vel,
                const components::Color &color);

void create_wheel(ecs::Context<Registry> &ctx, std::size_t car_id,
                  const glm::vec3 &position);

void create_shoe_item(ecs::Context<Registry> &ctx, std::size_t row_index,
                      std::size_t col_index);

void create_map(ecs::Context<Registry> &ctx);

void create_character(ecs::Context<Registry> &ctx);

void create_win_zone(ecs::Context<Registry> &ctx);
