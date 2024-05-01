#include "bounding_box.hpp"

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

BoundingBox::BoundingBox(const glm::vec2 &top_left,
                         const glm::vec2 &bottom_right)
    : top_left(top_left), bottom_right(bottom_right) {}

bool BoundingBox::intersect_with(const BoundingBox &other) const {
  return top_left[0] <= other.bottom_right[0] &&
         other.top_left[0] <= bottom_right[0] &&
         bottom_right[1] <= other.top_left[1] &&
         other.bottom_right[1] <= top_left[1];
}

bool BoundingBox::contains(const BoundingBox &other) const {
  return top_left[0] <= other.top_left[0] && top_left[1] >= other.top_left[1] &&
         bottom_right[0] >= other.bottom_right[0] &&
         bottom_right[1] <= other.bottom_right[1];
}

bool BoundingBox::contained_in(const BoundingBox &other) const {
  return other.contains(*this);
}

glm::vec2 BoundingBox::midpoint() const {
  return 0.5f * (top_left + bottom_right);
}

BoundingBox3D
BoundingBox3D::from_vertices(const std::vector<glm::vec3> &vertices) {
  float xmin = vertices[0][0], xmax = xmin, ymin = vertices[0][1], ymax = ymin,
        zmin = vertices[0][2], zmax = zmin;
  for (const auto &v : vertices) {
    xmin = std::min(xmin, v[0]);
    ymin = std::min(ymin, v[1]);
    zmin = std::min(zmin, v[2]);
    xmax = std::max(xmax, v[0]);
    ymax = std::max(ymax, v[1]);
    zmax = std::max(zmax, v[2]);
  }
  return {{xmin, ymin, zmin}, {xmax, ymax, zmax}};
}

BoundingBox3D::BoundingBox3D(const glm::vec3 &min_point,
                             const glm::vec3 &max_point)
    : min_point(min_point), max_point(max_point) {}

bool BoundingBox3D::intersect_with(const BoundingBox3D &other) const {
  return min_point[0] <= other.max_point[0] &&
         max_point[0] >= other.min_point[0] &&
         min_point[1] <= other.max_point[1] &&
         max_point[1] >= other.min_point[1] &&
         min_point[2] <= other.max_point[2] &&
         max_point[2] >= other.min_point[2];
}

bool BoundingBox3D::contains(const BoundingBox3D &other) const {
  return min_point[0] <= other.min_point[0] &&
         min_point[1] <= other.min_point[1] &&
         min_point[2] <= other.min_point[2] &&
         other.max_point[0] <= max_point[0] &&
         other.max_point[1] <= max_point[1] &&
         other.max_point[2] <= max_point[2];
}

bool BoundingBox3D::contained_in(const BoundingBox3D &other) const {
  return other.contains(*this);
}

glm::vec3 BoundingBox3D::midpoint() const {
  return 0.5f * (min_point + max_point);
}

BoundingBox3D BoundingBox3D::transform(const glm::mat4 &transform) const {
  const auto min_point_4d =
                 glm::vec4(min_point[0], min_point[1], min_point[2], 1),
             max_point_4d =
                 glm::vec4(max_point[0], max_point[1], max_point[2], 1);
  const auto transformed1 = glm::vec3(transform * min_point_4d),
             transformed2 = glm::vec3(transform * max_point_4d),
             min_point = glm::vec3(std::min(transformed1[0], transformed2[0]),
                                   std::min(transformed1[1], transformed2[1]),
                                   std::min(transformed1[2], transformed2[2])),
             max_point = glm::vec3(std::max(transformed1[0], transformed2[0]),
                                   std::max(transformed1[1], transformed2[1]),
                                   std::max(transformed1[2], transformed2[2]));
  return {min_point, max_point};
}
