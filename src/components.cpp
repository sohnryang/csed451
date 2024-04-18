#include <components.hpp>

#include <bounding_box.hpp>

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

using namespace components;

BoundingBox3D Mesh::bounding_box() const {
  return boudning_box_with_transform(glm::mat4(1));
}

BoundingBox3D
Mesh::boudning_box_with_transform(const glm::mat4 &transform) const {
  std::vector<glm::vec4> transformed;
  std::transform(
      vertices.begin(), vertices.end(), std::back_inserter(transformed),
      [this, transform](const glm::vec3 &vertex) {
        return mat * transform * glm::vec4(vertex[0], vertex[1], vertex[2], 1);
      });

  float xmin = transformed[0][0], xmax = xmin, ymin = transformed[0][1],
        ymax = ymin, zmin = transformed[0][2], zmax = zmin;
  for (const auto &v : transformed) {
    xmin = std::min(xmin, v[0]);
    ymin = std::min(ymin, v[1]);
    zmin = std::min(zmin, v[2]);
    xmax = std::max(xmax, v[0]);
    ymax = std::max(ymax, v[1]);
    zmax = std::max(zmax, v[2]);
  }
  return {{xmin, ymin, zmin}, {xmax, ymax, zmax}};
}
