cmake_minimum_required (VERSION 3.0)
project (Renderer)
set (CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

if (APPLE) 
  set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    glew
  )

  set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    /usr/local/Cellar/glew/2.1.0/include
    /usr/local/Cellar/glfw/3.2.1/include
    /usr/local/Cellar/glm/0.9.9.4/include
    # /usr/local/Cellar/glew/2.1.0/bin
    # /usr/local/Cellar/glfw/3.2.1/bin
    # /usr/local/Cellar/glew/2.1.0/lib
    # /usr/local/Cellar/glfw/3.2.1/lib
  )

  set(LINK_DIRS
    /usr/local/Cellar/glew/2.1.0/lib
    /usr/local/Cellar/glfw/3.2.1/lib
    /usr/local/Cellar/glm/0.9.9.4/lib
  )
  add_definitions(-DAPPLE)
else()
  set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
  )

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

include_directories(${INCLUDE_DIRS})
link_directories(${LINK_DIRS})

set(
  SRC
  src/main.cpp 
  src/engine.cpp 
  src/shaders.cpp 
  src/frame_buffer.cpp 
  src/sky_dome.cpp 
  src/texture.cpp 
  src/shaders.cpp
  src/clipmap.cpp
  src/terrain.cpp
  src/subregion.cpp
  src/cube.cpp 
  src/building.cpp 
)

add_executable(main ${SRC})
target_link_libraries(main ${ALL_LIBS})
