#pragma once

namespace ecs {
namespace systems {
template <class T> class System {
public:
  virtual void operator()(T &registry) = 0;
  virtual ~System();
};

template <class T> System<T>::~System() {}
} // namespace systems
} // namespace ecs
