#include <components.hpp>

#include <bounding_box.hpp>

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

using namespace components;

VertexContainer::~VertexContainer() {}

VertexVector::VertexVector(std::vector<glm::vec4> &&vertices)
    : _vertices(std::move(vertices)) {}

const std::vector<glm::vec4> &VertexVector::vertices() { return _vertices; }

CircleVertex::CircleVertex(glm::vec4 center, float radius, int point_count)
    : _center(center), _radius(radius), _point_count(point_count),
      _vertices(point_count) {}

const std::vector<glm::vec4> &CircleVertex::vertices() {
  const float PI = std::acos(-1);
  for (int i = 0; i < _point_count; i++) {
    float angle = 2 * PI / _point_count * i;
    const auto point =
        _radius * glm::vec4(std::cos(angle), std::sin(angle), 0, 0) + _center;
    _vertices[i] = point;
  }
  return _vertices;
}

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

BoundingBox RenderInfo::bounding_box() const {
  return bounding_box_with_trasform(glm::mat4(1));
}

BoundingBox
RenderInfo::bounding_box_with_trasform(const glm::mat4 &transform) const {
  std::vector<glm::vec4> transformed;
  const auto &vertices = this->vertex_container->vertices();
  std::transform(vertices.cbegin(), vertices.cend(),
                 std::back_inserter(transformed),
                 [this, transform](const glm::vec4 &vertex) {
                   return mat * transform * vertex;
                 });

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
