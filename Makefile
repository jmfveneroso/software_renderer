CC=g++

OS := $(shell uname)
$(info $(OS))
ifeq ($(OS), Linux)
  INC=-I/usr/local/include
  LIBS=-L/usr/X11/lib -L/usr/local/lib -lglfw3 -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -lGLEW -lpng
else
  LIBS=-L/usr/local/lib -lglfw -lglew -framework OpenGL -framework GLUT
endif

_DEPS = src/shaders.h src/bitmap.h src/objloader.h src/controls.h src/sphere.h src/cube.h src/render_object.h src/tangentspace.h

build/main: src/main.cpp $(_DEPS)
	$(CC) -o $@ $< $(LIBS) $(C_FLAGS) $(INC) -g -O3
