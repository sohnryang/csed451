#pragma once
#include <glm/glm.hpp>

#include <cstddef>

#include <bounding_box.hpp>

const std::size_t GRID_SIZE = 8;
const float STEP_SIZE = 2.0f;

BoundingBox3D grid_to_world(int row1, int col1, int row2, int col2);
