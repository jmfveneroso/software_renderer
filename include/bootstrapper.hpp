#ifndef _BOOTSTRAPPER_HPP_
#define _BOOTSTRAPPER_HPP_

#include "ioc_container.hpp"
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
    container.RegisterInstance<Input, Input, Window>();
    container.RegisterInstance<Player, Player>();
    container.RegisterInstance<EntityManager, EntityManager>();
    container.RegisterInstance<Renderer, Renderer, EntityManager>();
    container.RegisterInstance<Engine, Engine, Window, Renderer, Input>();
  }
};

} // End of namespace.

#endif
