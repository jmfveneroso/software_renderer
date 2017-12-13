#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <string>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif
