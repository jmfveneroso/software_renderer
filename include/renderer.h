#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <memory>
#include "window.h"
#include "entity_manager.hpp"
#include "entity.hpp"
#include "frame_buffer.hpp"
#include "shaders.h"
#include "player.hpp"

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
  std::shared_ptr<Window> window_;
  std::shared_ptr<EntityManager> entity_manager_;
  std::shared_ptr<Player> player_;

  Camera camera;
  std::vector< std::shared_ptr<IEntity> > render_entities_;

  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;

  void PushRenderEntity(const std::string&);
  void PopRenderEntity();

 public:
  Renderer(
    std::shared_ptr<Window>, 
    std::shared_ptr<EntityManager>,
    std::shared_ptr<Player>
  );

  void ComputeMatrices();
  void DrawScene(int, int, const std::string&);
  void SetReflectionCamera(float water_height);
  void Render();
  void Clean();
};

} // End of namespace.

#endif
