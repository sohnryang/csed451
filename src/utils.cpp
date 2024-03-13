#include "utils.hpp"

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

#include "components.hpp"

components::BoundingBox
bounding_box_of_transformed(const components::RenderInfo &render_info,
                            const components::Transform &transform) {
  const glm::mat4 mat = glm::translate(glm::mat4(1), transform.disp);
  std::vector<glm::vec4> transformed;
  const auto &vertices = render_info.vertices;
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

  return {glm::vec2(xmin, ymax), glm::vec2(xmax, ymin)};
}

bool intersect(const components::BoundingBox &box1,
               const components::BoundingBox &box2) {
  return box1.first[0] <= box2.second[0] && box2.first[0] <= box1.second[0] &&
         box1.second[1] <= box2.first[1] && box2.second[1] <= box1.first[1];
}
