#include "model.hpp"

#include <glm/glm.hpp>

#include <utility>
#include <vector>

#include <bounding_box.hpp>

Model::Model(std::vector<glm::vec3> &&vertices)
    : vertices(std::move(vertices)) {
  bounding_box = BoundingBox3D::from_vertices(this->vertices);
}
