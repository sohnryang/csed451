#pragma once

#include <glm/glm.hpp>

#include <queue>
#include <vector>

namespace components {
struct Color {
  float r;
  float g;
  float b;
};

struct RenderInfo {
  std::vector<glm::vec4> vertices;
  Color color;
};

struct Transform {
  glm::vec3 disp;
  glm::vec3 vel;
};

enum class ActionKind { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };

struct Character {
  std::queue<ActionKind> actions;
};

using BoundingBox = std::pair<glm::vec2, glm::vec2>;

struct ActionRestriction {
  BoundingBox bounding_box;
  std::vector<ActionKind> restrictions;
};

struct WinZone {
  BoundingBox bounding_box;
};

struct Car {};
} // namespace components
