#pragma once

#include <glm/glm.hpp>

struct BoundingBox {
  glm::vec2 top_left;
  glm::vec2 bottom_right;

  BoundingBox() = default;
  BoundingBox(const BoundingBox &) = default;
  BoundingBox(BoundingBox &&) = default;
  BoundingBox(const glm::vec2 &top_left, const glm::vec2 &bottom_right);
  BoundingBox &operator=(const BoundingBox &) = default;
  BoundingBox &operator=(BoundingBox &&) = default;

  bool intersect_with(const BoundingBox &other) const;
};
