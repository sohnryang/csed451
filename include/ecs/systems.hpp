#pragma once

#include "ecs/entities.hpp"
#include <vector>

namespace ecs {
template <class T> class Context;

namespace systems {
template <class T> class System {
private:
  virtual bool should_apply(Context<T> &ctx, entities::EntityId id);
  virtual void update_single(Context<T> &ctx, entities::EntityId id) = 0;
  virtual void pre_update(Context<T> &ctx);
  virtual void post_update(Context<T> &ctx);

public:
  virtual void operator()(Context<T> &ctx);
  virtual ~System();
};
} // namespace systems

template <class T> class Context {
private:
  entities::EntityManager _entity_manager;
  T _registry;
  std::vector<std::unique_ptr<systems::System<T>>> _systems;
  bool _loop_started;
  std::chrono::time_point<std::chrono::system_clock> _last_updated;

public:
  Context(T &&registry,
          std::vector<std::unique_ptr<systems::System<T>>> &&systems = {});
  Context(const Context &) = delete;
  Context(Context &&) = default;

  entities::EntityManager &entity_manager();
  T &registry();
  std::vector<std::unique_ptr<systems::System<T>>> &systems();
  std::chrono::time_point<std::chrono::system_clock> &last_updated();

  void update();
};

template <class T>
Context<T>::Context(T &&registry,
                    std::vector<std::unique_ptr<systems::System<T>>> &&systems)
    : _entity_manager(), _registry(registry), _systems(systems),
      _loop_started(false), _last_updated() {}

template <class T> entities::EntityManager &Context<T>::entity_manager() {
  return _entity_manager;
}

template <class T> T &Context<T>::registry() { return _registry; }

template <class T>
std::vector<std::unique_ptr<systems::System<T>>> &Context<T>::systems() {
  return _systems;
}

template <typename T>
std::chrono::time_point<std::chrono::system_clock> &Context<T>::last_updated() {
  return _last_updated;
}

template <class T> void Context<T>::update() {
  auto now = std::chrono::system_clock::now();
  if (!_loop_started) {
    _last_updated = now;
    _loop_started = true;
  }

  for (auto &s : _systems)
    (*s)(*this);

  _last_updated = now;
}

namespace systems {
template <class T>
bool System<T>::should_apply(Context<T> &ctx, entities::EntityId id) {
  return true;
}

template <class T> void System<T>::pre_update(Context<T> &ctx) {}

template <class T> void System<T>::post_update(Context<T> &ctx) {}

template <class T> void System<T>::operator()(Context<T> &ctx) {
  pre_update(ctx);

  for (entities::EntityId i = 0; i < ctx.entity_manager().end_id(); i++)
    if (should_apply(ctx, i))
      update_single(ctx, i);

  post_update(ctx);
}

template <class T> System<T>::~System() {}
} // namespace systems
} // namespace ecs
