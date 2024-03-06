#include "ecs/entities.hpp"

using namespace ecs::entities;

EntityManager::EntityManager() : current_id(0) {}

EntityId EntityManager::next_id() { return current_id++; }

EntityId EntityManager::end_id() const { return current_id; }
