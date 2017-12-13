#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <string>
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>

bool loadOBJ(
  const char * path, 
  std::vector<glm::vec3> & out_vertices, 
  std::vector<glm::vec2> & out_uvs,
  std::vector<glm::vec3> & out_normals
);

#endif
