#include "ecs/entities.hpp"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>

using namespace ecs::entities;

EntityManager::EntityManager() : EntityManager(4096) {}

EntityManager::EntityManager(EntityId max_entities)
    : _end_id(0), _max_entities(max_entities), _vacant_ids(), _entity_graph() {}

EntityId EntityManager::next_id() {
  EntityId new_id;
  if (!_vacant_ids.empty()) {
    const auto id = _vacant_ids.front();
    _vacant_ids.pop();
    new_id = id;
  } else if (_end_id >= _max_entities)
    throw std::length_error("Out of available entities");
  else
    new_id = _end_id++;

  _entity_graph[new_id] = {new_id, {}};
  return new_id;
}

EntityId EntityManager::end_id() const { return _end_id; }

void EntityManager::remove_id(EntityId id) {
  if (!_entity_graph.count(id))
    throw std::out_of_range("ID not found");

  const auto &node = _entity_graph[id];
  auto &parent_children = _entity_graph[node.parent].children;
  const auto it =
      std::find(parent_children.cbegin(), parent_children.cend(), id);
  parent_children.erase(it);
  _entity_graph.erase(id);
  _vacant_ids.push(id);
}

std::unordered_map<EntityId, EntityGraphNode> &EntityManager::entity_graph() {
  return _entity_graph;
}
