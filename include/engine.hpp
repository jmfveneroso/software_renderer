#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <exception>
#include <memory>
#include "window.h"
#include "renderer.h"
#include "input.hpp"
#include "physics.hpp"
#include "config.h"

namespace Sibyl {

class Engine {
  std::shared_ptr<Input> input_;
  std::shared_ptr<Window> window_;
  std::shared_ptr<Renderer> renderer_;
  std::shared_ptr<Physics> physics_;

 public:
  Engine(
    std::shared_ptr<Window>, 
    std::shared_ptr<Renderer>, 
    std::shared_ptr<Input>, 
    std::shared_ptr<Physics>
  );

  void Run();
};

} // End of namespace.

#endif
