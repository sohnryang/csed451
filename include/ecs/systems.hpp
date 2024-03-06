#pragma once

namespace ecs {
template <class T> class Context;

namespace systems {
template <class T> class System {
public:
  virtual void operator()(Context<T> &registry) = 0;
  virtual ~System();
};

template <class T> System<T>::~System() {}
} // namespace systems
} // namespace ecs
