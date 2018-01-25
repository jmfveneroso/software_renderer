#ifndef _BOOTSTRAPPER_HPP_
#define _BOOTSTRAPPER_HPP_

#include "ioc_container.hpp"
#include "physics.hpp"
#include "engine.hpp"
#include "player.hpp"
#include "entity_manager.hpp"
#include <GLFW/glfw3.h>

using namespace Sibyl;

namespace {

class Bootstrapper {
 public:
  static void Bootstrap() {
    static IoC::Container& container = IoC::Container::Get();
    container.RegisterInstance<Window, Window>();
    container.RegisterInstance<Player, Player>();
    container.RegisterInstance<EntityManager, EntityManager, Player>();
    container.RegisterInstance<Input, Input, Window, Player, EntityManager>();
    container.RegisterInstance<Physics, Physics, EntityManager, Player>();
    container.RegisterInstance<Renderer, Renderer, Window, EntityManager, Player>();
    container.RegisterInstance<Engine, Engine, Window, Renderer, Input, Physics>();
  }
};

} // End of namespace.

#endif
