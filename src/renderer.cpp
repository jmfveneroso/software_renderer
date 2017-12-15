#include "renderer.h"

namespace Sibyl {

Renderer::Renderer(std::shared_ptr<EntityManager> entity_manager)
  : entity_manager_(entity_manager) {
}

void Renderer::CreateScene(int windowWidth_, int windowHeight_) {
  windowWidth = windowWidth_;
  windowHeight= windowHeight_;

  reflection_water = FrameBuffer(1000, 750, vec2(0.0f, 0.0f));
  refraction_water = FrameBuffer(1000, 750, vec2(-1.0f, 0.0f));
  screen_fbo       = FrameBuffer(1000, 750, vec2(-1.0f, -1.0f));

  terrain = entity_manager_->GetEntity("terrain");
  sky_dome = entity_manager_->GetEntity("sky");

  GLuint waterProgramID = LoadShaders("shaders/vshade_water", "shaders/fshade_water");
  water = WaterRenderObject("res/water2.obj", reflection_water.GetTexture(), refraction_water.GetTexture(), "textures/water_dudv.bmp", "textures/water_normal.bmp", waterProgramID, refraction_water.GetDepthTexture());
}

void Renderer::computeMatricesFromInputs () {
  glm::vec3 direction(
    cos(verticalAngle) * sin(horizontalAngle), 
    sin(verticalAngle),
    cos(verticalAngle) * cos(horizontalAngle)
  );
  
  glm::vec3 right = glm::vec3(
    sin(horizontalAngle - 3.14f/2.0f) * 2, 
    0,
    cos(horizontalAngle - 3.14f/2.0f) * 2
  );

  glm::vec3 front = glm::vec3(
    cos(verticalAngle) * sin(horizontalAngle) * 2, 
    0,
    cos(verticalAngle) * cos(horizontalAngle) * 2
  );
  
  glm::vec3 up = glm::cross(right, direction);

  camera.position = position;
  camera.direction = direction;
  camera.up = up;

  float FoV = initialFoV; 
  
  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 2000 units
  ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 3000.0f);

  // Camera matrix
  ViewMatrix = glm::lookAt(
    camera.position,                    // Camera is here
    camera.position + camera.direction, // and looks here : at the same position, plus "direction"
    camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
  );
}

void Renderer::Render(int windowWidth, int windowHeight, GLuint framebuffer, vec4 plane, bool complete) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0, 0, windowWidth, windowHeight);

  WaterRenderObject::UpdateMoveFactor(1.0f/60.0f);

  // Render to the screen
  if (camera.position.y < WATER_HEIGHT) {
    glClearColor(0.0f, 0.2f, 0.3f, 0.0f);
  } else {
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (complete) {
    water.SetClipPlane(plane);

    glDisable(GL_CULL_FACE);
    water.Draw(ProjectionMatrix, ViewMatrix, camera.position);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
  }

  sky_dome.position = camera.position;
  sky_dome.position.y = 1.0;
  if (position.y > WATER_HEIGHT) 
    sky_dome.position.y = -50.0;

  ProjectionMatrix = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.1f, 10000.0f);
  sky_dome.SetClipPlane(plane);
  sky_dome.Draw(ProjectionMatrix, ViewMatrix, camera.position);
  terrain.SetClipPlane(plane);
  terrain.Draw(ProjectionMatrix, ViewMatrix, camera.position);
}

void Renderer::SetReflectionCamera(float water_height) {
  float distance = 2 * (camera.position.y - water_height);
  camera.position.y -= distance;
  camera.direction = glm::reflect(camera.direction, vec3(0, 1, 0));
  camera.up = -camera.up;

  // Camera matrix
  ViewMatrix = glm::lookAt(
    camera.position,                    // Camera is here
    camera.position + camera.direction, // and looks here : at the same position, plus "direction"
    camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
  );
}

void Renderer::DrawScene(GLFWwindow* window) {
  float water_height = WATER_HEIGHT;

  // Reflection surface.
  vec4 plane;
  if (camera.position.y < water_height)
    plane = vec4(0, -1, 0, water_height + 2.0f);
  else
    plane = vec4(0, 1, 0, -water_height + 1.0f);

  Camera old_camera = camera;
  glm::mat4 OldProjectionMatrix = ProjectionMatrix;
  SetReflectionCamera(water_height);
  Render(1000, 750, reflection_water.GetFramebuffer(), plane);
  camera = old_camera;

  ViewMatrix = glm::lookAt(
    camera.position,                    // Camera is here
    camera.position + camera.direction, // and looks here : at the same position, plus "direction"
    camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
  );
  ProjectionMatrix = OldProjectionMatrix;

  if (camera.position.y < water_height)
    plane = vec4(0, 1, 0, -water_height + 1.0f);
  else
    plane = vec4(0, -1, 0, water_height + 2.0f);

  Render(1000, 750, refraction_water.GetFramebuffer(), plane);

  plane = vec4(0, -1, 0, 10000);

  if (camera.position.y < water_height)
    terrain.SetWaterFog(true);
  else
    terrain.SetWaterFog(false);

  Render(windowWidth, windowHeight, screen_fbo.GetFramebuffer(), plane, true);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, windowWidth*2, windowHeight*2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  screen_fbo.Draw();

  computeMatricesFromInputs();
  glm::vec3 last_pos = position;
  UpdatePlayerPos(window);
  UpdateGravity(window);
  terrain.TestCollision(&position, last_pos);
}

void Renderer::Clean() {
  terrain.Clean();
  sky_dome.Clean();
  water.Clean();
}

} // End of namespace.
