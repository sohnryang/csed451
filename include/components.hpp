#pragma once

#include <glm/glm.hpp>

#include <queue>
#include <vector>

#include "bounding_box.hpp"

namespace components {
struct Color {
  float r;
  float g;
  float b;
};

struct Transform {
  glm::mat4 mat;
};

struct RenderInfo {
  std::vector<glm::vec4> vertices;
  Color color;

  BoundingBox bounding_box_with_transform(const Transform &transform) const;
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
  glm::vec3 disp;
  glm::vec3 vel;
};
} // namespace components
