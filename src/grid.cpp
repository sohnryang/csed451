#include <grid.hpp>

#include <glm/glm.hpp>

#include <bounding_box.hpp>

BoundingBox grid_to_world_cell(int x, int y) {
  const auto diagonal = glm::vec2(STEP_SIZE, -STEP_SIZE),
             top_left = glm::vec2(-STEP_SIZE * GRID_SIZE / 2.0 + STEP_SIZE * x,
                                  (0.5f + y) * STEP_SIZE);
  return {top_left, top_left + diagonal};
}
