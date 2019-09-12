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

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 
  glEnable(GL_CULL_FACE);
  glEnable(GL_CLIP_PLANE0);

  // Why is this necessary? Should look on shaders. 
  // Vertex arrays group VBOs.
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  glfwSetCharCallback(window_, terminal_->PressKey);
  glfwSetCharCallback(window_, TextEditor::PressCharCallback);
  glfwSetKeyCallback(window_, TextEditor::PressKeyCallback);
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
  GLuint diffuse_texture_id = LoadTexture("diffuse_terrain", "textures/dirt.bmp");
  GLuint sand_texture_id = LoadTexture("sand_terrain", "textures/sand.bmp");
  GLuint w_diffuse_texture_id = LoadTexture("water_dudv", "textures/water_dudv.bmp");
  GLuint w_normal_texture_id = LoadTexture("water_normal", "textures/water_normal.bmp");

  // Sky.
  sky_dome_ = make_shared<SkyDome>();

  terrain_ = make_shared<Terrain>(
    diffuse_texture_id, 
    sand_texture_id,
    w_diffuse_texture_id, 
    w_normal_texture_id
  );

  building_ = make_shared<Building>(0.125f, 5.0f, glm::vec3(2000.125f, 205.0f, 2000.0f));
  terminal_ = make_shared<Terminal>();
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
  glViewport(0, 0, window_width_, window_height_);
  glClearColor(0.3f, 0.5f, 0.6f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  sky_dome_->Draw(ProjectionMatrix, ViewMatrix, camera.position, player_.position);
  terrain_->Draw(ProjectionMatrix, ViewMatrix, camera.position, player_.position);
  building_->Draw(ProjectionMatrix, ViewMatrix, camera.position);

  switch (game_state_) {
    case TERMINAL:
      terminal_->Draw(player_.position);
      break;
    case TXT:
      if (TextEditor::Close())
        game_state_ = FREE;
      else
        TextEditor::Draw();
      break;
    default:
      break;
  }
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

void Engine::ProcessGameInput(){
  static double last_time = glfwGetTime();
  
  // Compute time difference between current and last frame.
  double current_time = glfwGetTime();
  float delta_time = float(current_time - last_time);

  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    Move(FORWARD, delta_time);

  // Move backward.
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    Move(BACK, delta_time);

  // Strafe right.
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    Move(RIGHT, delta_time);

  // Strafe left.
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    Move(LEFT, delta_time);

  if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
    if (building_->Interact(player_)) {
      game_state_ = TXT;
    }
  }

  float mouse_sensitivity = 0.005f;
  if (glfwGetKey(window_, GLFW_KEY_H) == GLFW_PRESS)
    player_.h_angle += mouse_sensitivity * 5;
  if (glfwGetKey(window_, GLFW_KEY_J) == GLFW_PRESS)
    player_.v_angle -= mouse_sensitivity * 5;
  if (glfwGetKey(window_, GLFW_KEY_K) == GLFW_PRESS)
    player_.v_angle += mouse_sensitivity * 5;
  if (glfwGetKey(window_, GLFW_KEY_L) == GLFW_PRESS)
    player_.h_angle -= mouse_sensitivity * 5;

  if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
    if (player_.can_jump) {
      player_.can_jump = false;
      player_.speed.y += 0.3f;
    }
  }

  if (glfwGetKey(window_, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) {
    if (terminal_->SetState(true)) {
      game_state_ = TERMINAL;
    }
  }

  double x_pos, y_pos;
  glfwGetCursorPos(window_, &x_pos, &y_pos);
  glfwSetCursorPos(window_, 0, 0);

  // Change orientation.
  player_.h_angle += mouse_sensitivity * float(-x_pos);
  player_.v_angle += mouse_sensitivity * float(-y_pos);
  if (player_.v_angle < -1.57f) player_.v_angle = -1.57f;
  if (player_.v_angle >  1.57f) player_.v_angle =  1.57f;

  last_time = current_time;
}

void Engine::ProcessTerminalInput(){
  if (glfwGetKey(window_, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) {
    if (terminal_->SetState(false)) {
      game_state_ = FREE;
    }
    return;
  }

  if (glfwGetKey(window_, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
    double current_time = glfwGetTime();
    if (pressed_backspace_at_ == 0.0) {
      terminal_->Backspace();
      pressed_backspace_at_ = current_time;
    } else if (current_time > pressed_backspace_at_ + TYPE_DELAY) {
      terminal_->Backspace();
      pressed_backspace_at_ += TYPE_SPEED;
    }
  } else {
    pressed_backspace_at_ = 0.0;
  }

  if (glfwGetKey(window_, GLFW_KEY_ENTER) == GLFW_PRESS) {
    double current_time = glfwGetTime();
    if (pressed_enter_at_ == 0.0) {
      terminal_->Execute(player_);
      pressed_enter_at_ = current_time;
    } else if (current_time > pressed_enter_at_ + TYPE_DELAY) {
      terminal_->Execute(player_);
      pressed_enter_at_ += TYPE_SPEED;
    }
  } else {
    pressed_enter_at_ = 0.0;
  }
}

void Engine::ProcessTextInput() {
  // if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
  //   TextEditor::GetInstance().Enable(game_state_, false);
  // }
}

void Engine::UpdateForces() {
  glm::vec3 prev_pos = player_.position;

  player_.speed += glm::vec3(0, -GRAVITY, 0);

  // Friction.
  player_.speed.x *= 0.9;
  player_.speed.y *= 0.99;
  player_.speed.z *= 0.9;

  player_.position += player_.speed;

  // Test collision with building.
  building_->Collide(player_.position, prev_pos, player_.can_jump, player_.speed);

  // Test collision with terrain.
  float height = terrain_->GetHeight(player_.position.x, player_.position.z);
  if (player_.position.y - player_.height < height) {
    glm::vec3 pos = player_.position;
    pos.y = height + player_.height;
    player_.position = pos;
    glm::vec3 speed = player_.speed;
    if (speed.y < 0) speed.y = 0.0f;
    player_.speed = speed;
    player_.can_jump = true;
  }
}

void Engine::Run() {
  CreateWindow();
  CreateEntities();

  double last_time = glfwGetTime();
  int frames = 0;
  // std::this_thread::sleep_for(std::chrono::seconds(1));
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

    switch (game_state_) {
      case TERMINAL:
        ProcessTerminalInput();
        terminal_->Update();
        break;
      case TXT:
        ProcessTextInput();
        break;
      case FREE:
      default:
        ProcessGameInput();
        break;
    }

    UpdateForces();

    // Swap buffers.
    glfwSwapBuffers(window_);
    glfwPollEvents();
  // } while (glfwGetKey(window_, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_) == 0);
  } while (glfwWindowShouldClose(window_) == 0);

  // Cleanup VBO and shader.
  for (auto it : shaders_)
    glDeleteProgram(it.second.program_id());

  for (auto it : textures_)
    glDeleteTextures(1, &it.second);

  // Close OpenGL window and terminate GLFW.
  glfwTerminate();
}

} // End of namespace.
