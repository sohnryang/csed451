#include <grid.hpp>

#include <glm/glm.hpp>

#include <bounding_box.hpp>

float grid_ticks_to_float(int x) { return (x - 4) * STEP_SIZE; }

glm::vec2 grid_to_world(int x, int y) {
  return glm::vec2(grid_ticks_to_float(x), grid_ticks_to_float(y));
}

BoundingBox grid_to_world_cell(int x, int y) {
  const auto diagonal = glm::vec2(STEP_SIZE, -STEP_SIZE),
             top_left =
                 glm::vec2(grid_ticks_to_float(x), grid_ticks_to_float(y + 1));
  return {top_left, top_left + diagonal};
}
