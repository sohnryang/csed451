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
  bool contains(const BoundingBox &other) const;
  bool contained_in(const BoundingBox &other) const;
  glm::vec2 midpoint() const;
};

struct BoundingBox3D {
  glm::vec3 min_point;
  glm::vec3 max_point;

  BoundingBox3D() = default;
  BoundingBox3D(const BoundingBox3D &) = default;
  BoundingBox3D(BoundingBox3D &&) = default;
  BoundingBox3D(const glm::vec3 &min_point, const glm::vec3 &max_point);
  BoundingBox3D &operator=(const BoundingBox3D &) = default;
  BoundingBox3D &operator=(BoundingBox3D &&) = default;

  bool intersect_with(const BoundingBox3D &other) const;
  bool contains(const BoundingBox3D &other) const;
  bool contained_in(const BoundingBox3D &other) const;
  glm::vec3 midpoint() const;
  BoundingBox3D transform(const glm::mat4 &transform);
};
