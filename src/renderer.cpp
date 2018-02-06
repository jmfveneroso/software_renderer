#include "renderer.h"

namespace Sibyl {

Renderer::Renderer(
  std::shared_ptr<Window> window, 
  std::shared_ptr<EntityManager> entity_manager,
  std::shared_ptr<Player> player
) : window_(window),
    entity_manager_(entity_manager),
    player_(player) {
}

void Renderer::ComputeMatrices() {
  float v_angle = player_->vertical_angle();
  float h_angle = player_->horizontal_angle();

  glm::vec3 direction(
    cos(v_angle) * sin(h_angle), 
    sin(v_angle),
    cos(v_angle) * cos(h_angle)
  );
  
  glm::vec3 right = glm::vec3(
    sin(h_angle - 3.14f/2.0f) * 2, 
    0,
    cos(h_angle - 3.14f/2.0f) * 2
  );

  glm::vec3 front = glm::vec3(
    cos(v_angle) * sin(h_angle) * 2, 
    0,
    cos(v_angle) * cos(h_angle) * 2
  );
  
  glm::vec3 up = glm::cross(right, direction);

  camera.position = player_->position();
  camera.direction = direction;
  camera.up = up;

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 2000 units
  ProjectionMatrix = glm::perspective(glm::radians(player_->fov()), 4.0f / 3.0f, 20.0f, 2000000.0f);

  // Camera matrix
  ViewMatrix = glm::lookAt(
    camera.position,                    // Camera is here
    camera.position + camera.direction, // and looks here : at the same position, plus "direction"
    camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
  );
}

void Renderer::DrawScene(int width, int height, const std::string& frame_buffer_name) {
  GLuint frame_buffer = entity_manager_->GetFrameBuffer(frame_buffer_name)->GetFramebuffer();
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto entity : render_entities_) {
    entity->Draw(ProjectionMatrix, ViewMatrix, camera.position);
  }
}

void Renderer::SetReflectionCamera(float water_height) {
  float distance = 2 * (camera.position.y - water_height);
  camera.position.y -= distance;
  camera.direction = glm::reflect(camera.direction, glm::vec3(0, 1, 0));
  camera.up = -camera.up;

  // Camera matrix
  ViewMatrix = glm::lookAt(
    camera.position,                    // Camera is here
    camera.position + camera.direction, // and looks here : at the same position, plus "direction"
    camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
  );
}

void Renderer::PushRenderEntity(const std::string& name) {
  render_entities_.push_back(entity_manager_->GetEntity(name));
}

void Renderer::PopRenderEntity() {
  render_entities_.pop_back();
}

void Renderer::Render() {
  PushRenderEntity("sky");

  glm::vec3 sky_position = camera.position;
  sky_position.y = 0.0;
  entity_manager_->GetEntity("sky")->set_position(sky_position);

  PushRenderEntity("pro_terrain");
  entity_manager_->GetTerrain()->SetClipPlane(glm::vec4(0, 1, 0, 100.0f));
  entity_manager_->GetTerrain()->DrawWater(false);

  Camera old_camera = camera;
  SetReflectionCamera(-100);
  DrawScene(600, 400, "reflection");
  camera = old_camera;

  entity_manager_->GetTerrain()->SetClipPlane(glm::vec4(0, -1, 0, 100.0f + 1.0f));
  entity_manager_->GetTerrain()->DrawWater(false);

  ComputeMatrices();
  DrawScene(600, 400, "refraction");

  PushRenderEntity("cube");

  entity_manager_->GetTerrain()->SetClipPlane(glm::vec4(0, 1, 0, 100.0f));
  entity_manager_->GetTerrain()->DrawWater(true);
  DrawScene(window_->width(), window_->height(), "screen");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_->width() * 2, window_->height() * 2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  entity_manager_->GetFrameBuffer("screen")->Draw();

  PopRenderEntity();
  PopRenderEntity();
  PopRenderEntity();
}

void Renderer::Clean() {
  entity_manager_->Clean();
}

} // End of namespace.
