#pragma once

#include <cstddef>
#include <queue>

namespace ecs {
namespace entities {
using EntityId = std::size_t;

class EntityManager {
private:
  EntityId _end_id;
  const EntityId _max_entities;
  std::queue<EntityId> _vacant_ids;

public:
  EntityManager();
  EntityManager(EntityId max_entities);
  EntityManager(const EntityManager &) = delete;
  EntityManager(EntityManager &&) = default;

  EntityId next_id();
  EntityId end_id() const;
  void remove_id(EntityId id);
};
} // namespace entities
} // namespace ecs
