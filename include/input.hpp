#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <memory>

using namespace glm;
#include "window.h"
#include "player.hpp"
#include "config.h"

namespace Sibyl {

class Input {
  std::shared_ptr<Window> window_;
  std::shared_ptr<Player> player_;

 public:
  Input(
    std::shared_ptr<Window>,
    std::shared_ptr<Player>
  );

  void ProcessInput();
};

} // End of namespace.

#endif
