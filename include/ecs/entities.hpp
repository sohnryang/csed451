#pragma once

#include <cstddef>

namespace ecs {
namespace entities {
using EntityId = size_t;

class EntityManager {
private:
  EntityId current_id;

public:
  EntityManager();

  EntityId next_id();
  EntityId end_id() const;
};
} // namespace entities
} // namespace ecs
