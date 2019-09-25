#include "engine.hpp"

namespace Sibyl {

Engine::Engine(
  shared_ptr<GameState> game_state, 
  shared_ptr<Renderer> renderer, 
  shared_ptr<EntityManager> entity_manager, 
  shared_ptr<TextEditor> text_editor
) : game_state_(game_state), 
    renderer_(renderer),
    entity_manager_(entity_manager), 
    text_editor_(text_editor) {
  CreateWindow();
}

void Engine::CreateWindow() {
  renderer_->CreateFramebuffer("screen", game_state_->width(), game_state_->height());
  renderer_->CreateFramebuffer("intersect", game_state_->width(), game_state_->height());
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

  entity_manager_->set_terrain(terrain_);
}

void Engine::Render() {
  game_state_->UpdateViewMatrix();

  ProjectionMatrix = game_state_->projection_matrix();
  ViewMatrix = game_state_->view_matrix();

  if (game_state_->mode() == FREE) {
    renderer_->SetFBO("intersect");
    renderer_->Clear(0, 0, 0);
    renderer_->SetFBO("screen");
    renderer_->Clear(0.3f, 0.5f, 0.6f);

    sky_dome_->Draw(ProjectionMatrix, ViewMatrix, camera.position, player_.position);
    terrain_->Draw(ProjectionMatrix, ViewMatrix, camera.position, player_.position);
    entity_manager_->Draw();
  }

  renderer_->DrawScreen(game_state_->mode() != FREE);

  switch (game_state_->mode()) {
    case TXT: {
      if (text_editor_->Close()) {
        game_state_->ChangeMode(FREE);
      } else {
        text_editor_->Draw();
      }
      break;
    }
    default:
      break;
  }
}

void Engine::ProcessGameInput(){
  static double last_time = glfwGetTime();

  KeyPress kp;
  while (game_state_->ReadKeyPress(&kp)) {
    if (kp.action == GLFW_PRESS) {
      switch (kp.key) {
        case GLFW_KEY_Q:
          text_editor_->Enable();
          text_editor_->OpenFile("/dev/create");
          game_state_->ChangeMode(TXT);
          break;
        // Interact with objects.
        case GLFW_KEY_E:
        case GLFW_KEY_ENTER:
          entity_manager_->Interact(kp.mods & GLFW_MOD_SHIFT);
          break;
      }
    }
  }
  
  // Compute time difference between current and last frame.
  double current_time = glfwGetTime();
  
  // Move forward.
  if (glfwGetKey(game_state_->window(), GLFW_KEY_W) == GLFW_PRESS)
    game_state_->MovePlayer(FORWARD);

  // Move backward.
  if (glfwGetKey(game_state_->window(), GLFW_KEY_S) == GLFW_PRESS)
    game_state_->MovePlayer(BACK);

  // Strafe right.
  if (glfwGetKey(game_state_->window(), GLFW_KEY_D) == GLFW_PRESS)
    game_state_->MovePlayer(RIGHT);

  // Strafe left.
  if (glfwGetKey(game_state_->window(), GLFW_KEY_A) == GLFW_PRESS)
    game_state_->MovePlayer(LEFT);

  float mouse_sensitivity = 0.025f;
  if (glfwGetKey(game_state_->window(), GLFW_KEY_H) == GLFW_PRESS)
    game_state_->Look(LEFT, mouse_sensitivity);

  if (glfwGetKey(game_state_->window(), GLFW_KEY_J) == GLFW_PRESS)
    game_state_->Look(DOWN, mouse_sensitivity);

  if (glfwGetKey(game_state_->window(), GLFW_KEY_K) == GLFW_PRESS)
    game_state_->Look(UP, mouse_sensitivity);

  if (glfwGetKey(game_state_->window(), GLFW_KEY_L) == GLFW_PRESS)
    game_state_->Look(RIGHT, mouse_sensitivity);

  if (glfwGetKey(game_state_->window(), GLFW_KEY_SPACE) == GLFW_PRESS) {
    game_state_->Jump();
  }

  double x_pos, y_pos;
  glfwGetCursorPos(game_state_->window(), &x_pos, &y_pos);
  glfwSetCursorPos(game_state_->window(), 0, 0);

  // Change orientation.
  Player& p = game_state_->player();
  p.h_angle += mouse_sensitivity * float(-x_pos);
  p.v_angle +=+ mouse_sensitivity * float(-y_pos);
  if (game_state_->player().v_angle < -1.57f) p.v_angle = -1.57f;
  if (game_state_->player().v_angle >  1.57f) p.v_angle = +1.57f;

  last_time = current_time;
}

void Engine::UpdateForces() {
  Player& p = game_state_->player();

  glm::vec3 prev_pos = p.position;

  p.speed += glm::vec3(0, -GRAVITY, 0);

  // Friction.
  p.speed.x *= 0.9;
  p.speed.y *= 0.99;
  p.speed.z *= 0.9;

  p.position += p.speed;

  // Test collision with building.
  entity_manager_->Collide(p.position, prev_pos, p.can_jump, p.speed);

  // Test collision with terrain.
  float height = terrain_->GetHeight(p.position.x, p.position.z);
  if (p.position.y - p.height < height) {
    glm::vec3 pos = p.position;
    pos.y = height + p.height;
    p.position = pos;
    glm::vec3 speed = p.speed;
    if (speed.y < 0) speed.y = 0.0f;
    p.speed = speed;
    p.can_jump = true;
  }
}

void Engine::Run() {
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

    UpdateForces();
    entity_manager_->Update();

    switch (game_state_->mode()) {
      case FREE:
        ProcessGameInput();
        break;
      default:
        break;
    }

    Render();

    // Swap buffers.
    glfwSwapBuffers(game_state_->window());
    glfwPollEvents();
  } while (glfwWindowShouldClose(game_state_->window()) == 0);

  // Cleanup VBO and shader.
  for (auto it : shaders_)
    glDeleteProgram(it.second.program_id());

  for (auto it : textures_)
    glDeleteTextures(1, &it.second);

  // Close OpenGL window and terminate GLFW.
  glfwTerminate();
}

} // End of namespace.
