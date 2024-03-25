#include "components.hpp"

#include "bounding_box.hpp"

#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

using namespace components;

BoundingBox
RenderInfo::bounding_box_with_transform(const Transform &transform) const {
  const glm::mat4 mat = glm::translate(glm::mat4(1), transform.disp);
  std::vector<glm::vec4> transformed;
  const auto &vertices = this->vertices;
  std::transform(vertices.cbegin(), vertices.cend(),
                 std::back_inserter(transformed),
                 [&mat](const glm::vec4 &vertex) { return mat * vertex; });

  float xmin = transformed[0][0], xmax = transformed[0][0],
        ymin = transformed[0][1], ymax = transformed[0][1];
  for (const auto &v : transformed) {
    xmin = std::min(xmin, v[0]);
    xmax = std::max(xmax, v[0]);
    ymin = std::min(ymin, v[1]);
    ymax = std::max(ymax, v[1]);
  }

  return {{xmin, ymax}, {xmax, ymin}};
}
