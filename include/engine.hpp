#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <exception>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <memory>

#include "terrain.hpp"
#include "sky_dome.hpp"
#include "cube.hpp"
#include "building.hpp"
#include "frame_buffer.hpp"
#include "texture.hpp"
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

struct Player {
  glm::vec3 position = glm::vec3(2000, 300, 2000);
  glm::vec3 next_position = glm::vec3(0, 0, 0);
  glm::vec3 speed = glm::vec3(0, 0, 0);
  float h_angle = 0.0f;
  float v_angle = 0.0f;
  float fov = PLAYER_FOV;
  float height = PLAYER_HEIGHT;
};

class Engine {
  GLFWwindow* window_;
  int window_width_ = WINDOW_WIDTH;
  int window_height_ = WINDOW_HEIGHT;

  Player player_;

  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  Camera camera;

  ShaderMap shaders_;
  TextureMap textures_;
  std::shared_ptr<Terrain> terrain_;
  std::shared_ptr<SkyDome> sky_dome_;
  std::shared_ptr<Cube> cube_;
  std::shared_ptr<Building> building_;
  std::shared_ptr<FrameBuffer> screen_;

  GLuint LoadTexture(const std::string&, const std::string&);
  void Move(Direction, float);
  void CreateWindow();
  void CreateEntities();
  void ProcessInput();
  void Render();
  void UpdateForces();

 public:
  Engine() {}

  void Run();
};

} // End of namespace.

#endif
