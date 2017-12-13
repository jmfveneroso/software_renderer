cmake_minimum_required (VERSION 3.0)
project (Renderer)

find_package(OpenGL REQUIRED)

include_directories(src)

set(ALL_LIBS
  ${OPENGL_LIBRARY}
  glfw
  glew
)

add_executable(main src/main.cpp src/engine.cpp src/controls.cpp src/vbo_indexer.cpp src/bitmap.cpp src/water_render_object.cpp src/objloader.cpp src/tangentspace.cpp src/shaders.cpp)
target_link_libraries(main ${ALL_LIBS})
