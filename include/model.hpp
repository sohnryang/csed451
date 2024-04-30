#pragma once

#include <glm/glm.hpp>

#include <vector>

#include <bounding_box.hpp>

struct Model {
  std::vector<glm::vec3> vertices;
  BoundingBox3D bounding_box;

  Model() = default;
  Model(const Model &) = default;
  Model(Model &&) = default;
  Model &operator=(const Model &) = default;
  Model &operator=(Model &&) = default;
  Model(std::vector<glm::vec3> &&vertices);
};
