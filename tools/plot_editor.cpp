#include "ioc_container.hpp"
#include "game_state.hpp"
#include "renderer.hpp"
#include "engine.hpp"

using namespace Sibyl;

void Run(
  shared_ptr<GameState> game_state_,  
  shared_ptr<TextEditor> text_editor_, 
  shared_ptr<Renderer> renderer_, 
  shared_ptr<Plotter> plotter_, 
  string filename
) {
  game_state_->ChangeMode(TXT);
  text_editor_->OpenFile(filename);
  text_editor_->Enable();
  renderer_->CreateFramebuffer("screen", game_state_->width(), game_state_->height());
  renderer_->CreateFramebuffer("plot", 1024, 1024);
  plotter_->UpdatePlot(filename, "plot");

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

    renderer_->SetFBO("screen");
    renderer_->Clear(0.3f, 0.5f, 0.6f);
    // text_editor_->Draw();

    // renderer_->DrawFBO("plot", ivec2(100, 100));

    renderer_->DrawScreen(false);

    // Swap buffers.
    glfwSwapBuffers(game_state_->window());
    glfwPollEvents();
  } while (!text_editor_->Close() && glfwWindowShouldClose(game_state_->window()) == 0);

  // Close OpenGL window and terminate GLFW.
  glfwTerminate();
}

int main(int argc, char** argv) {
  if (argc < 2) return 1;

  static IoC::Container& container = IoC::Container::Get();
  container.RegisterInstance<GameState, GameState>();
  container.RegisterInstance<Renderer, Renderer>();
  container.RegisterInstance<TextEditor, TextEditor, GameState, Renderer>();
  container.RegisterInstance<Plotter, Plotter, GameState, Renderer, TextEditor>();

  shared_ptr<GameState> game_state = container.Resolve<GameState>();
  shared_ptr<TextEditor> text_editor = container.Resolve<TextEditor>();
  shared_ptr<Renderer> renderer = container.Resolve<Renderer>();
  shared_ptr<Plotter> plotter = container.Resolve<Plotter>();
  Run(game_state, text_editor, renderer, plotter, string(argv[1]));
  return 0;
}
