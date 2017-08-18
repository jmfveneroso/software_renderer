# CC=gcc
# 
# INC=-I/usr/X11/include/ -I/usr/local/include
# LIBS=-L/usr/X11/lib -L/usr/local/lib -lX11 -lXext -lpng
# 
# _DEPS = bsp_tree.h vector.h level.h render.h player.h visual_engine.h sector.h textures.h
# # DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
# 
# main: main.c $(_DEPS)
# 	$(CC) -o $@ $< $(LIBS) $(C_FLAGS) $(INC) -g


CC=g++

OS := $(shell uname)
$(info $(OS))
ifeq ($(OS), Linux)
  INC=-I/usr/local/include
  LIBS=-L/usr/X11/lib -L/usr/local/lib -lglfw3 -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -lGLEW -lpng
else
  LIBS=-L/usr/X11/lib -L/usr/local/lib -lglfw -lglew -lpng -framework OpenGL -framework GLUT
endif



_DEPS = bsp_tree.h vector.h level.h render.h alternative_render.h player.h visual_engine.h sector.h textures.h

main: main.cpp $(_DEPS)
	$(CC) -o $@ $< $(LIBS) $(C_FLAGS) $(INC) -g -O3
