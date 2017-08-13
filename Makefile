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

#GLFW=-I/Users/joaomateusdefreitasveneroso/Downloads/opengl/ogl-master/external/glfw-3.1.2/include/GLFW -L/Users/joaomateusdefreitasveneroso/Downloads/opengl/ogl-master/build/external/glfw-3.1.2/src -lglfw3
#GLEW=-lglew
LIBS=-L/usr/X11/lib -L/usr/local/lib -lglfw -lglew -lpng -framework OpenGL -framework GLUT

_DEPS = bsp_tree.h vector.h level.h render.h player.h visual_engine.h sector.h textures.h

main: main.cpp $(_DEPS)
	$(CC) -o $@ $< $(LIBS) $(C_FLAGS) $(INC) -g -O3
