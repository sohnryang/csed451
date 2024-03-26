#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <queue>
#include <vector>

#include "bounding_box.hpp"

namespace components {
struct Color {
  float r;
  float g;
  float b;
};

class VertexContainer {
public:
  virtual const std::vector<glm::vec4> &vertices() const = 0;
  virtual ~VertexContainer();
};

class VertexVector : public VertexContainer {
private:
  std::vector<glm::vec4> _vertices;

public:
  VertexVector() = default;
  VertexVector(const VertexVector &) = delete;
  VertexVector(VertexVector &&) = default;
  VertexVector &operator=(VertexVector &&) = default;
  VertexVector(std::vector<glm::vec4> &&vertices);

  const std::vector<glm::vec4> &vertices() const override;
};

struct RenderInfo {
  std::unique_ptr<VertexContainer> vertex_container;
  Color color;
  glm::mat4 mat;

  BoundingBox bounding_box() const;
};

enum class ActionKind { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };

struct Character {
  std::queue<ActionKind> actions;
};

struct ActionRestriction {
  BoundingBox bounding_box;
  std::vector<ActionKind> restrictions;
};

struct WinZone {
  BoundingBox bounding_box;
};

struct Car {
  glm::vec3 vel;
};
} // namespace components
