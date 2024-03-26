#include "grid.hpp"

#include <glm/glm.hpp>

#include "bounding_box.hpp"

glm::vec2 grid_to_world(int x, int y) {
  return glm::vec2((x - 4) * STEP_SIZE, (y - 4) * STEP_SIZE);
}

BoundingBox grid_to_world_cell(int x, int y) {
  const auto diagonal = glm::vec2(STEP_SIZE, -STEP_SIZE),
             top_left = glm::vec2((x - 4) * STEP_SIZE, (y - 3) * STEP_SIZE);
  return {top_left, top_left + diagonal};
}
