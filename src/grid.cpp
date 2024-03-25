#include "grid.hpp"

#include <glm/glm.hpp>

#include "bounding_box.hpp"

glm::vec2 grid_to_world(int x, int y) {
  return glm::vec2((x - 4) * step_size, (y - 4) * step_size);
}

BoundingBox grid_to_world_cell(int x, int y) {
  const auto diagonal = glm::vec2(step_size, -step_size),
             top_left = glm::vec2((x - 4) * step_size, (y - 3) * step_size);
  return {top_left, top_left + diagonal};
}
