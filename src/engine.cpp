#include "engine.hpp"

namespace Sibyl {

void Engine::CreateWindow() {
  if (!glfwInit()) throw "Failed to initialize GLFW";

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // To make MacOS happy; should not be needed.
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

  window_ = glfwCreateWindow(window_width_, window_height_, APP_NAME, NULL, NULL);
  if (window_ == NULL) {
    glfwTerminate();
    throw "Failed to open GLFW window";
  }

  // We would expect width and height to be 1024 and 768
  // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, 
  // so we get the actual framebuffer size:
  glfwGetFramebufferSize(window_, &window_width_, &window_height_);
  glfwMakeContextCurrent(window_);

  // Needed for core profile.
  glewExperimental = true; 
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    throw "Failed to initialize GLEW";
  }

  // Hide the mouse and enable unlimited movement.
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwPollEvents();
  glfwSetCursorPos(window_, window_width_ / 2, window_height_ / 2);

  glClearColor(0.3f, 0.5f, 0.6f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 
  glEnable(GL_CULL_FACE);
  glEnable(GL_CLIP_PLANE0);

  // Why is this necessary? Should look on shaders.
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
}

GLuint Engine::LoadTexture(
  const string& name, 
  const string& texture_file_path
) {
  Texture texture = Texture(texture_file_path);
  textures_[name] = texture.texture_id();
  return texture.texture_id();
}

void Engine::CreateEntities() {
  // GLSL shaders.
  shaders_["static_screen"] = Shader("static_screen", "v_static_screen", "f_static_screen");
  shaders_["terrain"] = Shader("terrain", "v_terrain", "f_terrain", "g_terrain");
  shaders_["water"] = Shader("water", "v_water", "f_water");
  shaders_["sky"] = Shader("sky", "v_sky", "f_sky");
  shaders_["cube"] = Shader("cube", "v_cube", "f_cube", "g_cube");

  // Frame buffer.
  screen_ = make_shared<FrameBuffer>(shaders_["static_screen"], window_width_, window_height_);

  // Procedural terrain.
  GLuint diffuse_texture_id = LoadTexture("diffuse_terrain", "textures/dirt.bmp");
  GLuint sand_texture_id = LoadTexture("sand_terrain", "textures/sand.bmp");
  GLuint w_diffuse_texture_id = LoadTexture("water_dudv", "textures/water_dudv.bmp");
  GLuint w_normal_texture_id = LoadTexture("water_normal", "textures/water_normal.bmp");

  // Sky.
  sky_dome_ = make_shared<SkyDome>(shaders_["sky"]);

  terrain_ = make_shared<Terrain>(
    shaders_["terrain"],
    shaders_["water"],
    diffuse_texture_id, 
    sand_texture_id,
    w_diffuse_texture_id, 
    w_normal_texture_id
  );

  cube_ = make_shared<Cube>(shaders_["cube"]);
  building_ = make_shared<Building>(shaders_["cube"], 0.125f, 5.0f, glm::vec3(2000.125f, 205.0f, 2000.0f));
  float sx = 0.125f;
  float sz = 5.0f;

}

void Engine::Render() {
  glm::vec3 direction(
    cos(player_.v_angle) * sin(player_.h_angle), 
    sin(player_.v_angle),
    cos(player_.v_angle) * cos(player_.h_angle)
  );
  
  glm::vec3 right = glm::vec3(
    sin(player_.h_angle - 3.14f/2.0f), 
    0,
    cos(player_.h_angle - 3.14f/2.0f)
  );

  glm::vec3 front = glm::vec3(
    cos(player_.v_angle) * sin(player_.h_angle), 
    0,
    cos(player_.v_angle) * cos(player_.h_angle)
  );
  
  glm::vec3 up = glm::cross(right, direction);

  camera.position = player_.position;
  camera.direction = direction;
  camera.up = up;

  ProjectionMatrix = glm::perspective(glm::radians(player_.fov), 4.0f / 3.0f, NEAR_CLIPPING, FAR_CLIPPING);

  // Camera matrix
  ViewMatrix = glm::lookAt(
    camera.position,                    // Camera is here
    camera.position + camera.direction, // and looks here : at the same position, plus "direction"
    camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
  );

  // Draw.
  GLuint frame_buffer = screen_->GetFramebuffer();
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  glViewport(0, 0, window_width_, window_height_);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // sky_dome_->Draw(ProjectionMatrix, ViewMatrix, camera.position, player_.position);
  terrain_->Draw(ProjectionMatrix, ViewMatrix, camera.position, player_.position);
  cube_->Draw(ProjectionMatrix, ViewMatrix, camera.position);
  building_->Draw(ProjectionMatrix, ViewMatrix, camera.position);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_width_ * 2, window_height_ * 2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  screen_->Draw();
}

void Engine::Move(Direction direction, float delta_time) {
  glm::vec3 right = glm::vec3(
    sin(player_.h_angle - 3.14f/2.0f), 
    0,
    cos(player_.h_angle - 3.14f/2.0f)
  );

  glm::vec3 front = glm::vec3(
    sin(player_.h_angle), 
    0,
    cos(player_.h_angle)
  );
  
  switch (direction) {
    case FORWARD:
      player_.speed += front * delta_time * PLAYER_SPEED;
      // player_.position += front * delta_time * PLAYER_SPEED;
      break;
    case BACK:
      player_.speed -= front * delta_time * PLAYER_SPEED;
      break;
    case RIGHT:
      player_.speed += right * delta_time * PLAYER_SPEED;
      break;
    case LEFT:
      player_.speed -= right * delta_time * PLAYER_SPEED;
      break;
    default:
      break;
  }
}

void Engine::ProcessInput(){
  static double last_time = glfwGetTime();
  
  // Compute time difference between current and last frame
  double current_time = glfwGetTime();
  float delta_time = float(current_time - last_time);

  // player_->set_last_position(player_->position());
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    Move(FORWARD, delta_time);

  // Move backward
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    Move(BACK, delta_time);

  // Strafe right
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    Move(RIGHT, delta_time);

  // Strafe left
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    Move(LEFT, delta_time);

  if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS)
    player_.speed.y += 0.1f;

  double x_pos, y_pos;
  glfwGetCursorPos(window_, &x_pos, &y_pos);
  glfwSetCursorPos(window_, 1024 / 2, 768 / 2);

  // Change orientation.
  float mouse_speed = 0.005f;
  player_.h_angle += mouse_speed * float(1024 / 2 - x_pos);
  player_.v_angle   += mouse_speed * float(768 / 2 - y_pos);
  if (player_.v_angle < -1.57f) player_.v_angle = -1.57f;
  if (player_.v_angle >  1.57f) player_.v_angle =  1.57f;

  last_time = current_time;
}

void Engine::UpdateForces() {
  glm::vec3 prev_pos = player_.position;

  player_.speed += glm::vec3(0, -0.05, 0);

  // Decay.
  player_.speed *= 0.9;

  player_.position += player_.speed;

  // Test collision with terrain.
  float height = terrain_->GetHeight(player_.position.x, player_.position.z);
  if (player_.position.y - player_.height < height) {
    glm::vec3 pos = player_.position;
    pos.y = height + player_.height;
    player_.position = pos;
    glm::vec3 speed = player_.speed;
    if (speed.y < 0) speed.y = 0.0f;
    player_.speed = speed;
  }

  // Test collision with building.
  building_->Collide(player_.position, prev_pos);
}

void Engine::Run() {
  CreateWindow();
  CreateEntities();

  double last_time = glfwGetTime();
  int frames = 0;
  do {
    // Measure speed.
    double current_time = glfwGetTime();
    frames++;

    // If last printf() was more than 1 second ago.
    if (current_time - last_time >= 1.0) { 
      cout << 1000.0 / double(frames) << " ms/frame" << endl;
      frames = 0;
      last_time += 1.0;
    }

    Render();
    ProcessInput();
    UpdateForces();

    // Swap buffers.
    glfwSwapBuffers(window_);
    glfwPollEvents();
  } while (glfwGetKey(window_, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_) == 0);

  // Cleanup VBO and shader.
  for (auto it : shaders_)
    glDeleteProgram(it.second.program_id());

  for (auto it : textures_)
    glDeleteTextures(1, &it.second);

  // Close OpenGL window and terminate GLFW.
  glfwTerminate();
}

} // End of namespace.
