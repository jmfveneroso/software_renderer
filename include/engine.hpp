#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <vector>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <exception>
#include <memory>

using namespace glm;
#include "window.h"
#include "renderer.h"
#include "input.hpp"
#include "physics.hpp"

#include "shaders.h"
#include "bitmap.h"
#include "tangentspace.h"
#include "objloader.h"
#include "vbo_indexer.h"
#include "render_object.h"
#include "water.h"
#include "config.h"

extern glm::vec3 position;

namespace Sibyl {

class Engine {
  std::shared_ptr<Input> input_;
  std::shared_ptr<Window> window_;
  std::shared_ptr<Renderer> renderer_;
  std::shared_ptr<Physics> physics_;

 public:
  Engine(std::shared_ptr<Window>, std::shared_ptr<Renderer>, std::shared_ptr<Input>, std::shared_ptr<Physics>);

  void Run();
};

} // End of namespace.

#endif
