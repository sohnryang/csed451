#pragma once
#include <glm/glm.hpp>

#include <cstddef>

#include "bounding_box.hpp"

const std::size_t GRID_SIZE = 8;
const float STEP_SIZE = 2.0f / GRID_SIZE;

glm::vec2 grid_to_world(int x, int y);

BoundingBox grid_to_world_cell(int x, int y);
