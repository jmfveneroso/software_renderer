#include "engine.hpp"

namespace Sibyl {

Engine::Engine() : window_name_("Test"), 
  window_width_(600), window_height_(400) {
}

void Engine::Clean() {
  // Cleanup VBO and shader.
  renderer_.Clean();

  // Close OpenGL window and terminate GLFW.
  glfwTerminate();
}

void Engine::CreateWindow() {
  if (!glfwInit()) throw "Failed to initialize GLFW";

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // To make MacOS happy; should not be needed.
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

  window_ = glfwCreateWindow(window_width_, window_height_, window_name_, NULL, NULL);
  if (window_ == NULL) {
    Clean();
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
    Clean();
    throw "Failed to initialize GLEW";
  }

  // Hide the mouse and enable unlimited movement.
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwPollEvents();
  glfwSetCursorPos(window_, window_width_ / 2, window_height_ / 2);

  glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 
  glEnable(GL_CULL_FACE);
  glEnable(GL_CLIP_PLANE0);

  // Why is this necessary? Should look on shaders.
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
}

int Engine::Run() {
  CreateWindow();

  Renderer renderer(window_width_, window_height_);
  renderer.CreateScene();

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

    renderer.DrawScene(window_);

    // Swap buffers
    glfwSwapBuffers(window_);
    glfwPollEvents();

  // Check if the ESC key was pressed or the window was closed
  } while (
    glfwGetKey(window_, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window_) == 0 
  );

  Clean();
}

} // End of namespace.
