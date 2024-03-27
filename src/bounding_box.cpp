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
