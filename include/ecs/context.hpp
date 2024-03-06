#pragma once

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <memory>
#include <vector>

namespace ecs {
template <class T> class Context {
private:
  entities::EntityManager _entity_manager;
  T _registry;
  std::vector<std::unique_ptr<systems::System<T>>> _systems;

public:
  Context(T &&registry,
          std::vector<std::unique_ptr<systems::System<T>>> &&systems = {});
  Context(const Context &) = delete;
  Context(Context &&) = default;

  entities::EntityManager &entity_manager();
  T &registry();
  std::vector<std::unique_ptr<systems::System<T>>> &systems();

  void update();
};

template <class T>
Context<T>::Context(T &&registry,
                    std::vector<std::unique_ptr<systems::System<T>>> &&systems)
    : _registry(registry), _systems(systems) {}

template <class T> entities::EntityManager &Context<T>::entity_manager() {
  return _entity_manager;
}

template <class T> T &Context<T>::registry() { return _registry; }

template <class T>
std::vector<std::unique_ptr<systems::System<T>>> &Context<T>::systems() {
  return _systems;
}

template <class T> void Context<T>::update() {
  for (auto &system : _systems)
    system();
}
} // namespace ecs
