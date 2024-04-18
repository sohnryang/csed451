#include <bounding_box.hpp>

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
