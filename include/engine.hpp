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

#include "game_state.hpp"
#include "terrain.hpp"
#include "sky_dome.hpp"
#include "entity_manager.hpp"
#include "building.hpp"
#include "texture.hpp"
#include "renderer.hpp"
#include "shaders.h"
#include "config.h"

using namespace std;

namespace Sibyl {

typedef std::map<std::string, GLuint> TextureMap;
typedef std::map<std::string, Shader> ShaderMap;

class Engine {
  double pressed_backspace_at_ = 0.0;
  double pressed_enter_at_ = 0.0;
  GameMode game_mode_ = FREE;

  Player player_;

  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  Camera camera;

  ShaderMap shaders_;
  TextureMap textures_;

  shared_ptr<GameState> game_state_;
  shared_ptr<Renderer> renderer_;
  shared_ptr<EntityManager> entity_manager_;
  shared_ptr<TextEditor> text_editor_;
  shared_ptr<Terrain> terrain_;
  shared_ptr<SkyDome> sky_dome_;

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
  Engine(shared_ptr<GameState>, shared_ptr<Renderer>, shared_ptr<EntityManager>, shared_ptr<TextEditor>);

  void Run();
};

} // End of namespace.

#endif
