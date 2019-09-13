#include "building.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

Object::Object(
  glm::vec3 position,
  GLfloat rotation,
  const string& mesh_name 
) : position_(position),
    rotation_(rotation),
    mesh_name_(mesh_name) {
}

void Object::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  Graphics::GetInstance().DrawMesh(mesh_name_, ProjectionMatrix, ViewMatrix, camera, position_, rotation_);
}

Scroll::Scroll(
  glm::vec3 position,
  GLfloat rotation,
  const std::string& filename
) : position(position),
    object_(position, rotation, "scroll"), 
    filename(filename) {
}

Floor::Floor(
  glm::vec3 position,
  float width,
  float height,
  float length
) : position_(position), 
    width_(width), 
    height_(height),
    length_(length) {
}

void Floor::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  vec3 dimensions(width_, length_, height_);
  Graphics::GetInstance().Cube(ProjectionMatrix, ViewMatrix, camera, position_, dimensions, 0.0);
}

void Floor::Collide(glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed) {
  // Player bounding box.
  BoundingBox p = BoundingBox(
    player_pos.x - 0.35, 
    player_pos.y - 1.5, 
    player_pos.z - 0.35, 
    0.7, 1.5, 0.7
  );

  // AABB collision.
  if (
      p.x >= position_.x + width_  || p.x + p.width  <= position_.x ||
      p.y >= position_.y + height_ || p.y + p.height <= position_.y ||
      p.z >= position_.z + length_ || p.z + p.length <= position_.z
  ) return;

  // Collision vector.
  vec3 collision = player_pos - prev_pos;

  // Collision point between the player and each box face in collision 
  // vector coordinates.
  vector<double> collision_magnitude {
    (position_.y + height_ + 1.50 - prev_pos.y) / collision.y, // Top.
    (position_.y - prev_pos.y                 ) / collision.y, // Bottom.
    (position_.x + width_  + 0.35 - prev_pos.x) / collision.x, // Right.
    (position_.x - 0.35 - prev_pos.x          ) / collision.x, // Left.
    (position_.z + length_ + 0.35 - prev_pos.z) / collision.z, // Front.
    (position_.z - 0.35 - prev_pos.z          ) / collision.z  // Back.
  };

  // Select the face for which the collision displacement is minimum
  // when moving the player only along the collision vector.
  int min_index = -1;
  double min_value = numeric_limits<double>::max();
  for (int i = 0; i < 6; i++) {
    double k = collision_magnitude[i];
    if (k < 0.0 || k > 1.0) continue;

    if (min_index == -1 || k < min_value) {
      min_index = i;
      min_value = k;
    }
  }

  // Displace the player such that it is no longer in collision with
  // the selected face.
  switch (min_index) {
    case 0:
      can_jump = true;
      speed.y = 0;
    case 1:
      player_pos.y = prev_pos.y + min_value * collision.y;
      player_pos.y += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    case 2:
    case 3:
      player_pos.x = prev_pos.x + min_value * collision.x;
      player_pos.x += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    case 4:
    case 5:
      player_pos.z = prev_pos.z + min_value * collision.z;
      player_pos.z += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    default:
      break;
  }
}

Building::Building(
  float sx, 
  float sz,
  glm::vec3 position
) : shader_("building"), shader2_("terminal"), position_(position), sx_(sx), sz_(sz) {
  float s = 14.0f;
  float t = 0.25f;
  CreateFloor(vec3(1995, 205,   1995), s, true );
  CreateFloor(vec3(1995, 211.5, 1995), s, false);
  CreateFloor(vec3(1995, 218,   1995), s, false);

  vec3 pos = vec3(1995, 205, 1995);
  floors_.push_back(Floor(pos + vec3(-t,  19.5, -t  ), s+1+2*t, 1, t  ));
  floors_.push_back(Floor(pos + vec3(s+1, 19.5, 0   ), t,       1, s+1));
  floors_.push_back(Floor(pos + vec3(-t,  19.5, s+1 ), s+1+2*t, 1, t  ));
  floors_.push_back(Floor(pos + vec3(-t,  19.5, 0   ), t,       1, s+1));

  floors_.push_back(Floor(pos + vec3(4.25,  0, 2), 6.75, 1, 0.5));
  floors_.push_back(Floor(pos + vec3(4,  0, 2), 0.25, 4, 13));

  floors_.push_back(Floor(pos + vec3(14, 0, 5), 1, 1, 9));

  objects_.push_back(Object(vec3(2009, 205, 1996), radians(-90.0f), "book_stand"));
  scrolls_.push_back(Scroll(vec3(2009.25, 206, 2000.5), radians(-90.0f), "files/scroll_1.txt"));
  scrolls_.push_back(Scroll(vec3(2009.25, 206, 2001.5), radians(-90.0f), "files/scroll_2.txt"));
  scrolls_.push_back(Scroll(vec3(2009.25, 206, 2002.5), radians(-90.0f), "files/scroll_3.txt"));
  scrolls_.push_back(Scroll(vec3(2009.25, 206, 2003.5), radians(-90.0f), "files/scroll_4.txt"));
  scrolls_.push_back(Scroll(vec3(2009.25, 206, 2004.5), radians(-90.0f), "files/scroll_5.txt"));

  paintings_.push_back(WallPainting("files/plot1.txt", vec3(1995.75, 207.5, 1995), 0.0f));
  paintings_.push_back(WallPainting("files/plot2.txt", vec3(1995, 207.5, 1998), radians(90.0f)));
  paintings_.push_back(WallPainting("files/plot3.txt", vec3(1995, 207.5, 2001), radians(90.0f)));

  paintings_[0].LoadFile();
  paintings_[1].LoadFile();
  paintings_[2].LoadFile();

  glGenBuffers(1, &quad_vbo_);

  vector<vec3> vertices = {
    { 100, WINDOW_HEIGHT - 50, 0 },
    { 100, 50, 0 },
    { WINDOW_WIDTH-100, WINDOW_HEIGHT - 50, 0 },
    { WINDOW_WIDTH-100, WINDOW_HEIGHT - 50, 0 },
    { 100, 50, 0 },
    { WINDOW_WIDTH-100, 50, 0 }
  };

  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
}

void Building::CreateFloor(glm::vec3 position, float s, bool door) {
  float t = 0.25;

  if (door) {
    floors_.push_back(Floor(position + vec3(-t,    0, -t), s/2+t, 6, t));
    floors_.push_back(Floor(position + vec3(s/2+1, 0, -t), s/2+t, 6, t));
    floors_.push_back(Floor(position + vec3(s/2,   2, -t), 1,     4, t));
  } else {
    floors_.push_back(Floor(position + vec3(-t, 0, -t), s+1+2*t, 6, t));
  }

  floors_.push_back(Floor(position + vec3(s+1, 0, 0),     t, 6, s/2));
  floors_.push_back(Floor(position + vec3(s+1, 0, s/2+1), t, 6, s/2));
  floors_.push_back(Floor(position + vec3(s+1, 4, s/2),   t, 2, 1  ));
  floors_.push_back(Floor(position + vec3(s+1, 0, s/2),   t, 1, 1  ));

  floors_.push_back(Floor(position + vec3(-t, 0, s+1), s+1+2*t, 6, t));

  floors_.push_back(Floor(position + vec3(-t, 0, 0    ), t, 6, s/2));
  floors_.push_back(Floor(position + vec3(-t, 0, s/2+1), t, 6, s/2));
  floors_.push_back(Floor(position + vec3(-t, 4, s/2  ), t, 2, 1  ));
  floors_.push_back(Floor(position + vec3(-t, 0, s/2  ), t, 1, 1  ));

  floors_.push_back(Floor(position + vec3(-t,    6, -t   ), s+1+2*t, 0.5, s-1+t));
  floors_.push_back(Floor(position + vec3(-t,    6, s-1-t), s-1-6+t, 0.5, 2+2*t));
  floors_.push_back(Floor(position + vec3(s-1-6, 6, s-1+2), 6,       0.5, t    ));
  floors_.push_back(Floor(position + vec3(s-1,   6, s-1  ), 2+t,     0.5, 2+t  ));
 
  // Stairs.
  float x = 0;
  for (int i = 0; i < 12; i++) {
    x += 0.5;
    floors_.push_back(Floor(position + vec3(s-1 - x, 6 - x, s-1), 0.5, 0.5, 2));
  }
}

void Building::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  for (auto& p : paintings_)
    p.Draw(ProjectionMatrix, ViewMatrix, camera);

  for (auto& f : floors_)
    f.Draw(ProjectionMatrix, ViewMatrix, camera);

  for (auto& o : objects_)
    o.Draw(ProjectionMatrix, ViewMatrix, camera);

  for (auto& s : scrolls_)
    s.object().Draw(ProjectionMatrix, ViewMatrix, camera);
}

void Building::Collide(glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed) {
  for (auto& f : floors_)
    f.Collide(player_pos, prev_pos, can_jump, speed);
}

bool Building::Interact(Player& player) {
  for (auto& s : scrolls_) {
    if (distance2(player.position, s.position) < 1.0) {
      TextEditor::Enable();
      TextEditor::OpenFile(s.filename);
      return true;
    }
  }

  for (auto& p : paintings_) {
    if (distance2(player.position, p.position()) < 4.0) {
      p.LoadFile();
      TextEditor::Enable();
      TextEditor::OpenFile(p.filename());
      return true;
    }
  }
  return false;
}

} // End of namespace.
