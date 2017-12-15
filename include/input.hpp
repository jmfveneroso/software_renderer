#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <memory>

using namespace glm;
#include "window.h"
#include "config.h"

namespace Sibyl {

class Input {
  std::shared_ptr<Window> window_;
  bool over_ground_;
  glm::vec3 position_;
  float horizontal_angle_;
  float vertical_angle_;
  float initial_fov_;
  float speed_;
  glm::vec3 fall_speed_;
  float mouse_speed_;

 public:
  Input(std::shared_ptr<Window>);

  void UpdateGravity();
  void ProcessInput();
};

} // End of namespace.

#endif
