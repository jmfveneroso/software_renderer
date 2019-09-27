#include "plot.hpp"

using namespace std;
using namespace glm;
using namespace boost::filesystem;

namespace Sibyl {

Plot::Plot(
  unsigned int id,
  string filename,
  glm::vec3 position,
  GLfloat rotation
) : Object(id, position, rotation, "2d_plot"),
    filename(filename) {
}

} // End of namespace.
