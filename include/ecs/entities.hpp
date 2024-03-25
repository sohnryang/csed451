#pragma once

#include <cstddef>
#include <queue>
#include <unordered_map>
#include <vector>

namespace ecs {
namespace entities {
using EntityId = std::size_t;

struct EntityGraphNode {
  EntityId parent;
  std::vector<EntityId> children;
};

class EntityManager {
private:
  EntityId _end_id;
  const EntityId _max_entities;
  std::queue<EntityId> _vacant_ids;
  std::unordered_map<EntityId, EntityGraphNode> _entity_graph;

public:
  EntityManager();
  EntityManager(EntityId max_entities);
  EntityManager(const EntityManager &) = delete;
  EntityManager(EntityManager &&) = default;

  EntityId next_id();
  EntityId end_id() const;
  void remove_id(EntityId id);
  std::unordered_map<EntityId, EntityGraphNode> &entity_graph();
};
} // namespace entities
} // namespace ecs
