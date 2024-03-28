#pragma once

#include <glm/glm.hpp>

#include <map>
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

// TODO: use iterator instead of this stop-gap solution
class VertexContainer {
public:
  virtual const std::vector<glm::vec4> &vertices() = 0;
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

  const std::vector<glm::vec4> &vertices() override;
};

class CircleVertex : public VertexContainer {
private:
  glm::vec4 _center;
  float _radius;
  int _point_count;
  std::vector<glm::vec4> _vertices;

public:
  CircleVertex() = default;
  CircleVertex(const CircleVertex &) = delete;
  CircleVertex(CircleVertex &&) = default;
  CircleVertex &operator=(CircleVertex &&) = default;
  CircleVertex(glm::vec4 center, float radius, int point_count = 64);

  const std::vector<glm::vec4> &vertices() override;
};

struct RenderInfo {
  std::unique_ptr<VertexContainer> vertex_container;
  Color color;
  glm::mat4 mat;

  BoundingBox bounding_box() const;
  BoundingBox bounding_box_with_trasform(const glm::mat4 &transform) const;
};

enum class ActionKind { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, WEAR_SHOE };

struct Character {
  static constexpr float DEFAULT_ANIMATION_DURATION = 0.2f;
  ActionKind current_action;
  std::queue<ActionKind> actions;
  float speed_multipler = 1.0f;
};

struct ActionRestriction {
  BoundingBox bounding_box;
  std::vector<ActionKind> restrictions;
  bool ignore_passthrough;
};

struct WinZone {
  BoundingBox bounding_box;
};

struct Car {
  glm::vec3 vel;
};

enum class AnimationKind { DISABLED, ONCE, LOOP };

struct AnimationInfo {
  AnimationKind kind;
  std::map<float, glm::mat4> keyframes;
};

enum class AnimationState { BEFORE_START, RUNNING, FINISHED };

struct Animation {
  AnimationState state;
  AnimationInfo info;
  glm::mat4 mat;
  float time_elapsed;
};

struct ShoeItem {
  static constexpr float MULTIPLIER = 2.0f;
};
} // namespace components
