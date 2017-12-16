#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <memory>
#include "entity_manager.hpp"
#include "water_render_object.h"
#include "render_object.h"
#include "water.h"
#include "shaders.h"
#include "controls.h"

#define WATER_HEIGHT 0.0 

extern float verticalAngle;
extern float horizontalAngle;
extern float initialFoV;
extern glm::vec3 position;

namespace Sibyl {

struct Camera {
  glm::vec3 position;
  glm::vec3 up;
  glm::vec3 direction;
};

class Renderer {
  std::shared_ptr<EntityManager> entity_manager_;
  Camera camera;
  int windowWidth, windowHeight;
  std::shared_ptr<IEntity> terrain;
  std::shared_ptr<IEntity> sky_dome;
  WaterRenderObject water;
  FrameBuffer reflection_water;
  FrameBuffer refraction_water;
  FrameBuffer screen_fbo;
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;

 public:
  Renderer(std::shared_ptr<EntityManager>);

  void CreateScene(int windowWidth_, int windowHeight_);
  void computeMatricesFromInputs();
  void Render(int windowWidth, int windowHeight, GLuint framebuffer, vec4 plane, bool complete = false);
  void SetReflectionCamera(float water_height);
  void DrawScene(GLFWwindow* window);
  void Clean();
};

} // End of namespace.

#endif
