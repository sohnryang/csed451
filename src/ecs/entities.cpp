#include "ecs/entities.hpp"
#include <stdexcept>

using namespace ecs::entities;

EntityManager::EntityManager() : EntityManager(4096) {}

EntityManager::EntityManager(EntityId max_entities)
    : _end_id(0), _max_entities(max_entities), _vacant_ids() {}

EntityId EntityManager::next_id() {
  if (!_vacant_ids.empty()) {
    const auto id = _vacant_ids.front();
    _vacant_ids.pop();
    return id;
  }
  if (_end_id >= _max_entities)
    throw std::length_error("Out of available entities");
  return _end_id++;
}

EntityId EntityManager::end_id() const { return _end_id; }

void EntityManager::remove_id(EntityId id) { _vacant_ids.push(id); }
