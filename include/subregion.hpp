#ifndef _SUBREGION_HPP_
#define _SUBREGION_HPP_

#include <vector>
#include <memory>
#include <fstream>
#include <cstring>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include "shaders.h"
#include "player.hpp"
#include "height_map.hpp"
#include "config.h"

#define CLIPMAP_SIZE 258
#define CLIPMAP_OFFSET ((CLIPMAP_SIZE - 2) / 2)
#define LEFT_BORDER   8
#define TOP_BORDER    4
#define BOTTOM_BORDER 2
#define RIGHT_BORDER  1

namespace Sibyl {

enum SubregionLabel {
  SUBREGION_LEFT = 0,
  SUBREGION_TOP,
  SUBREGION_BOTTOM,
  SUBREGION_RIGHT,
  SUBREGION_CENTER
};

class Subregion {
  static const short BORDERS[];

  std::shared_ptr<Player> player_;
  SubregionLabel subregion_;
  GLuint buffer_[2][2];
  int buffer_size_[2][2];
  glm::ivec2 top_left_[2][2];
  glm::ivec2 size_[2][2];
  float min_height_[2][2];
  float max_height_[2][2];
  int clipmap_level_;

  void Init();
  void CreateTopBorder(std::vector<unsigned int>&, int, int);
  void CreateBottomBorder(std::vector<unsigned int>&, int, int);
  void CreateLeftBorder(std::vector<unsigned int>&, int, int);
  void CreateRightBorder(std::vector<unsigned int>&, int, int);
  void CreateBuffer(glm::ivec2);
  void CreateTile(std::vector<unsigned int>&, int, int);

  void DrawElements(glm::ivec2, glm::vec2, glm::vec2, glm::vec2);
  Segment ClipSubregion(glm::ivec2, glm::vec2, glm::vec2, float);
  Segment ClipSegToFrustum(Segment);
  Segment GetEnclosingSegment(Segment, Segment, bool);

 public:
  Subregion() {}
  Subregion(std::shared_ptr<Player>, SubregionLabel, int);

  void Draw(glm::ivec2, glm::vec2);
  void UpdateHeight(int, int, float);
  void Clear();
};

} // End of namespace.

#endif
