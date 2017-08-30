#ifndef RENDERER_H
#define RENDERER_H

#include "water_render_object.h"

struct Camera {
  glm::vec3 position;
  glm::vec3 up;
  glm::vec3 direction;
};

class Renderer {
  Camera camera;
  int windowWidth, windowHeight;
  RenderObject terrain;
  RenderObject sky_dome;
  WaterRenderObject water;
  Water reflection_water;
  Water refraction_water;
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;

 public:
  Renderer(int windowWidth, int windowHeight) 
    : windowWidth(windowWidth), windowHeight(windowHeight), 
     reflection_water(windowWidth, windowHeight, vec2(0.0f, 0.0f)), 
     refraction_water(windowWidth, windowHeight, vec2(-1.0f, 0.0f)) {}

  void CreateScene() {
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("shaders/vshade_normals", "shaders/fshade_normals");
    GLuint waterProgramID = LoadShaders("shaders/vshade_water", "shaders/fshade_water");

    terrain = RenderObject("res/medium_terrain.obj", "textures/medium_terrain.bmp", "textures/normal.bmp", "textures/specular_orange.bmp", programID, false);
    sky_dome = RenderObject("res/skydome.obj", "textures/skydome.bmp", "textures/normal.bmp", "textures/specular_orange.bmp", programID, true);
    water = WaterRenderObject("res/water.obj", reflection_water.GetTexture(), refraction_water.GetTexture(), "textures/water_dudv.bmp", waterProgramID);
  }

  void computeMatricesFromInputs () {
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
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 2000.0f);

    // Camera matrix
    ViewMatrix = glm::lookAt(
      camera.position,                    // Camera is here
      camera.position + camera.direction, // and looks here : at the same position, plus "direction"
      camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
    );
  }

  void Render(int windowWidth, int windowHeight, GLuint framebuffer, vec4 plane, bool complete = false) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, windowWidth, windowHeight);

    WaterRenderObject::UpdateMoveFactor(1/60.0f);

    // Render to the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (complete) {
      water.SetClipPlane(plane);
      water.Draw(ProjectionMatrix, ViewMatrix);
    }

    sky_dome.position = position;
    sky_dome.SetClipPlane(plane);
    sky_dome.Draw(ProjectionMatrix, ViewMatrix);
    terrain.SetClipPlane(plane);
    terrain.Draw(ProjectionMatrix, ViewMatrix);
  }

  void SetReflectionCamera(float water_height) {
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

  void DrawScene() {
    float water_height = 12.6f;

    // Reflection surface.
    vec4 plane = vec4(0, 1, 0, -water_height);
    Camera old_camera = camera;
    glm::mat4 OldProjectionMatrix = ProjectionMatrix;
    SetReflectionCamera(water_height);
    Render(windowWidth, windowHeight, reflection_water.GetFramebuffer(), plane);
    camera = old_camera;

    ViewMatrix = glm::lookAt(
      camera.position,                    // Camera is here
      camera.position + camera.direction, // and looks here : at the same position, plus "direction"
      camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
    );
    ProjectionMatrix = OldProjectionMatrix;

    plane = vec4(0, -1, 0, water_height);
    Render(windowWidth, windowHeight, refraction_water.GetFramebuffer(), plane);

    plane = vec4(0, -1, 0, 1000);
    Render(windowWidth, windowHeight, 0, plane, true);
    reflection_water.Draw();
    refraction_water.Draw();

    glm::vec3 last_pos = position;
    UpdatePlayerPos();
    UpdateGravity();

    terrain.TestCollision(&position, last_pos);
    computeMatricesFromInputs();
  }

  void Clean() {
    terrain.Clean();
    sky_dome.Clean();
    water.Clean();
  }
};

#endif
