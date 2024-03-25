#pragma once
#include <glm/glm.hpp>

#include <cstddef>

#include "bounding_box.hpp"

const std::size_t grid_size = 8;
const float step_size = 2.0f / grid_size;

glm::vec2 grid_to_world(int x, int y);

BoundingBox grid_to_world_cell(int x, int y);
