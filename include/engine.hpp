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
  glm::vec3 position = glm::vec3(0, 300, 0);
  glm::vec3 speed = glm::vec3(0, 0, 0);
  float h_angle = 0.0f;
  float v_angle = 0.0f;
  float fov = 45.0f;
  float height = 2000.0f;
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
