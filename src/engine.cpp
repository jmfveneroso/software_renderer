#include "engine.hpp"

namespace Sibyl {

Engine::Engine(
  std::shared_ptr<Window> window,
  std::shared_ptr<Renderer> renderer,
  std::shared_ptr<Input> input
) : window_(window), renderer_(renderer), input_(input) {
}

int Engine::Run() {
  renderer_->CreateScene(window_->width(), window_->height());

  double last_time = glfwGetTime();
  int nb_frames = 0;
  do {
    // Measure speed.
    double current_time = glfwGetTime();
    nb_frames++;

    // If last prinf() was more than 1 second ago.
    if (current_time - last_time >= 1.0) { 
      std::printf("%f ms/frame\n", 1000.0 / double(nb_frames));
      nb_frames = 0;
      last_time += 1.0;
    }

    renderer_->DrawScene(window_->window());
    window_->SwapBuffers();

  // Check if the ESC key was pressed or the window was closed
  } while (!window_->ShouldClose());

  // Cleanup VBO and shader.
  renderer_->Clean();

  // Close OpenGL window and terminate GLFW.
  window_->Close();
}

} // End of namespace.
