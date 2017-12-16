cmake_minimum_required (VERSION 3.0)
project (Renderer)
set (CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

include_directories(include)

if (APPLE) 
  set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    glew
  )
else()
  link_directories(/usr/X11/lib /usr/local/lib)
  set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw3
    GL 
    GLEW
    X11
    Xrandr 
    Xi 
    Xxf86vm 
    pthread
  )
endif()

set(
  SRC
  src/main.cpp 
  src/window.cpp 
  src/input.cpp 
  src/player.cpp 
  src/engine.cpp 
  src/physics.cpp 
  src/shaders.cpp 
  src/entity_manager.cpp 
  src/renderer.cpp
  src/controls.cpp 
  src/vbo_indexer.cpp 
  src/bitmap.cpp 
  src/water_render_object.cpp 
  src/objloader.cpp 
  src/tangentspace.cpp 
  src/shaders.cpp
)

add_executable(main ${SRC})
target_link_libraries(main ${ALL_LIBS})
