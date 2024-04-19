#include <grid.hpp>

#include <glm/glm.hpp>

#include <bounding_box.hpp>

#include <algorithm>

BoundingBox3D grid_to_world(int row1, int col1, int row2, int col2) {
  glm::vec3 top_left = {(-0.5 * GRID_SIZE + col1) * STEP_SIZE, 0,
                        (0.5 - row1) * STEP_SIZE},
            bottom_right = {(-0.5 * GRID_SIZE + col2 + 1) * STEP_SIZE, 1,
                            (0.5 - row2 - 1) * STEP_SIZE};
  if (top_left[2] > bottom_right[2])
    std::swap(top_left[2], bottom_right[2]);
  return BoundingBox3D(top_left, bottom_right);
}
