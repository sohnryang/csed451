#pragma once

#include <glm/glm.hpp>

#include "components.hpp"

components::BoundingBox
bounding_box_of_transformed(const components::RenderInfo &render_info,
                            const components::Transform &transform);

bool intersect(const components::BoundingBox &box1,
               const components::BoundingBox &box2);
