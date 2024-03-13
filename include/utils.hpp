#pragma once

#include <glm/glm.hpp>

#include <utility>

#include "components.hpp"

using BoundingBox = std::pair<glm::vec2, glm::vec2>;

BoundingBox
bounding_box_of_transformed(const components::RenderInfo &render_info,
                            const components::Transform &transform);

bool intersect(const BoundingBox &box1, const BoundingBox &box2);
