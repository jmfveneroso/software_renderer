#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <exception>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <memory>
#include <thread>
#include <chrono>

#include "terrain.hpp"
#include "sky_dome.hpp"
#include "building.hpp"
#include "texture.hpp"
#include "terminal.hpp"
#include "shaders.h"
#include "config.h"

using namespace std;

namespace Sibyl {

typedef std::map<std::string, GLuint> TextureMap;
typedef std::map<std::string, Shader> ShaderMap;

enum Direction {
  FORWARD,
  BACK,
  LEFT,
  RIGHT
};

struct Camera {
  glm::vec3 position;
  glm::vec3 up;
  glm::vec3 direction;
};

class Engine {
  GLFWwindow* window_;
  int window_width_ = WINDOW_WIDTH;
  int window_height_ = WINDOW_HEIGHT;
  double pressed_backspace_at_ = 0.0;
  double pressed_enter_at_ = 0.0;
  GameState game_state_ = FREE;

  Player player_;

  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  Camera camera;

  ShaderMap shaders_;
  TextureMap textures_;
  std::shared_ptr<Terrain> terrain_;
  std::shared_ptr<SkyDome> sky_dome_;
  std::shared_ptr<Building> building_;
  std::shared_ptr<Terminal> terminal_;

  GLuint LoadTexture(const std::string&, const std::string&);
  void Move(Direction, float);
  void CreateWindow();
  void CreateEntities();
  void ProcessGameInput();
  void ProcessTerminalInput();
  void ProcessTextInput();
  void Render();
  void UpdateForces();

 public:
  Engine() {}

  void Run();
};

} // End of namespace.

#endif
