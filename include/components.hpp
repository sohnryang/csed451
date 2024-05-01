#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <map>
#include <queue>
#include <vector>

#include "bounding_box.hpp"

namespace components {
struct Mesh {
  std::size_t model_index;
  glm::mat4 mat;
};

enum class ActionKind {
  MOVE_FORWARD,
  MOVE_BACK,
  MOVE_LEFT,
  MOVE_RIGHT,
  WEAR_SHOE
};

struct Character {
  static constexpr float DEFAULT_ANIMATION_DURATION = 0.2f;
  ActionKind current_action;
  std::queue<ActionKind> actions;
  float speed_multipler = 1.0f;
  BoundingBox3D model_bb;
};

struct ActionRestriction {
  BoundingBox3D bounding_box;
  std::vector<ActionKind> restrictions;
  bool ignore_passthrough;
};

struct WinZone {
  BoundingBox3D bounding_box;
};

struct Car {
  static constexpr float TRUCK_PLATE_DURATION = 0.7f;
  glm::vec3 vel;
  BoundingBox3D model_bb;
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
  BoundingBox3D model_bb;
};

struct CameraConfig {
  glm::vec3 eye;
  glm::vec3 center;
  glm::vec3 up;
  float fovy;
  float aspect_ratio;
  float znear;
  float zfar;
};
} // namespace components
