#pragma once

#include <queue>
#include <vector>

#include <glm/glm.hpp>

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

struct ActionRestriction {
  glm::vec2 top_left;
  glm::vec2 bottom_right;
  std::vector<ActionKind> restrictions;
};

struct Car {

};
} // namespace components
