#include "wall_painting.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

WallPainting::WallPainting(
  string filename,
  glm::vec3 position,
  GLfloat rotation
) : filename_(filename),
    shader_("painting", "v_painting", "f_painting"), 
    shader2_("lines", "v_lines", "f_lines"), 
    position_(position),
    rotation_(rotation),
    texture_size_(1024, 1024) {
  Init();
}

void WallPainting::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);

  float w = size_;
  float h = size_;
  float t = 0.1;

  vector<vec3> v {
    // Back face.
    vec3(0, 0, 0), vec3(w, 0, 0), vec3(0, -h, 0), vec3(w, -h, 0),
    // Front face.
    vec3(0, 0, t), vec3(w, 0, t), vec3(0, -h, t), vec3(w, -h, t),
  };

  glUseProgram(shader_.program_id());

  glm::mat4 ModelMatrix = glm::rotate(glm::mat4(1.0f), rotation_, glm::vec3(0.0, 1.0, 0.0));

  for (size_t i = 0; i < v.size(); ++i) {
    vec4 aux = ModelMatrix * vec4(v[i], 1.0f);
    v[i] = vec3(aux);
  }

  vertices_ = {
    v[0], v[4], v[1], v[1], v[4], v[5], // Top.
    v[1], v[3], v[0], v[0], v[3], v[2], // Back.
    v[0], v[2], v[4], v[4], v[2], v[6], // Left.
    v[5], v[7], v[1], v[1], v[7], v[3], // Right.
    v[4], v[6], v[5], v[5], v[6], v[7], // Front.
    v[6], v[2], v[7], v[7], v[2], v[3]  // Bottom.
  };

  vector<vec2> u = {
    vec2(0, 0), vec2(0, 1), vec2(1, 0), vec2(1, 1)
  };

  vector<glm::vec2> uvs {
    u[0], u[0], u[0], u[0], u[0], u[0], // Top.
    u[0], u[0], u[0], u[0], u[0], u[0], // Back.
    u[0], u[0], u[0], u[0], u[0], u[0], // Left.
    u[0], u[0], u[0], u[0], u[0], u[0], // Right.
    u[0], u[1], u[2], u[2], u[1], u[3], // Front.
    u[0], u[0], u[0], u[0], u[0], u[0], // Bottom.
  };

  for (int i = 0; i < 36; i++) indices_.push_back(i);

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_); 
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );
  
  // FRAME BUFFER.

  // texture_ = Texture("textures/dirt.bmp");

  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture_size_.x, texture_size_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &frame_buffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
  
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw;

  glGenBuffers(1, &vbo);

  BeginDraw();
  EndDraw();
}

vec3 WallPainting::GetColor(string color_name) {
  unordered_map<string, vec3> colors {
    { "red",  vec3(1, 0, 0) },
    { "green",  vec3(0, 1, 0) },
    { "blue",  vec3(0, 0, 1) },
    { "yellow",  vec3(1, 1, 0) },
    { "magenta",  vec3(1, 0, 1) }
  };
  return colors[color_name];
}

void WallPainting::LoadFile() {
  cout << filename_ << endl;
  ifstream f(filename_);
  if (!f.is_open()) return;
  string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());

  vector<string> lines;
  boost::split(lines, content, boost::is_any_of("\n"));

  BeginDraw();
  boost::regex re("(\"[^\"]+\")|[#A-Za-z0-9.-]+");
  for (int i = 0; i < lines.size(); i++) {
    boost::sregex_token_iterator j(lines[i].begin(), lines[i].end(), re, 0);
    boost::sregex_token_iterator k;
 
    vector<string> tkns;
    while (j != k) {
      tkns.push_back(*j++);
    }

    if (tkns.size() == 0) continue;

    string command = tkns[0];
    if (command == "#") continue;
    if (command == "Arrow" || command == "Line") {
      vec2 p1; 
      vec2 p2;
      GLfloat thickness;
      vec3 color;

      p1.x = boost::lexical_cast<float>(tkns[1]); 
      p1.y = boost::lexical_cast<float>(tkns[2]);

      p2.x = boost::lexical_cast<float>(tkns[3]); 
      p2.y = boost::lexical_cast<float>(tkns[4]);

      thickness = boost::lexical_cast<float>(tkns[5]);

      color = GetColor(tkns[6]);
      if (command == "Arrow")
        DrawArrow(p1, p2, thickness, color);
      else
        DrawLine(p1, p2, thickness, color);
    }

    if (command == "Point") {
      vec2 point; 
      GLfloat thickness;
      vec3 color;

      point.x = boost::lexical_cast<float>(tkns[1]); 
      point.y = boost::lexical_cast<float>(tkns[2]);
      thickness = boost::lexical_cast<float>(tkns[3]);
      color = GetColor(tkns[4]);
      DrawPoint(point, thickness, color);
    }

    if (command == "Text") {
      string text;
      vec2 point; 
      vec3 color;

      text = tkns[1].substr(1, tkns[1].size() - 2);
      point.x = boost::lexical_cast<float>(tkns[2]); 
      point.y = boost::lexical_cast<float>(tkns[3]);
      color = GetColor(tkns[4]);

      DrawText(text, point, color);
    }
  }
  EndDraw();
}

void WallPainting::DrawLine(
  vec2 p1, vec2 p2, GLfloat thickness, vec3 color
) {
  GLfloat s = thickness / 2.0f;
  vec2 step = normalize(p2 - p1);

  vector<vec2> v {
    p1 + s * (vec2(-step.y, step.x)), p1 + s * (vec2(step.y, -step.x)),
    p2 + s * (vec2(-step.y, step.x)), p2 + s * (vec2(step.y, -step.x))
  };

  glUniform3f(shader2_.GetUniformId("lineColor"), color.x, color.y, color.z);

  std::vector<glm::vec3> lines = {
    vec3(v[0], 0), vec3(v[1], 0), vec3(v[2], 0), vec3(v[2], 0), vec3(v[1], 0), vec3(v[3], 0)
  };

  shader2_.BindBuffer(vbo, 0, 3);
  glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(glm::vec3), &lines[0]); 

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void WallPainting::DrawArrow(
  vec2 p1, vec2 p2, GLfloat thickness, vec3 color
) {
  p1 *= pixels_per_step_;
  p2 *= pixels_per_step_;

  GLfloat height = 12.0f;
  GLfloat width = 5.0f;
  GLfloat steepness = 0.75f;

  vec2 step = normalize(p2 - p1);
  DrawLine(p1, p2 - step * (height * steepness), thickness, color);

  vector<vec2> v {
    p2, 
    (p2 - step * (height * steepness)),
    (p2 - step * height) + (width * vec2(-step.y, step.x)),
    (p2 - step * height) + (width * vec2(step.y, -step.x))
  };

  glUniform3f(shader2_.GetUniformId("lineColor"), color.x, color.y, color.z);

  std::vector<glm::vec3> lines = {
    vec3(v[0], 0), vec3(v[1], 0), vec3(v[2], 0),
    vec3(v[0], 0), vec3(v[1], 0), vec3(v[3], 0)
  };

  shader2_.BindBuffer(vbo, 0, 3);
  glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(glm::vec3), &lines[0]); 

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void WallPainting::DrawPoint(
  vec2 point, GLfloat thickness, vec3 color
) {
  GLfloat s = thickness / 2.0f;
  vector<vec2> v {
    point + vec2(-s, -s), point + vec2(-s, s), 
    point + vec2(s, -s), point + vec2(s, s)
  };

  glUniform3f(shader2_.GetUniformId("lineColor"), color.x, color.y, color.z);

  std::vector<glm::vec3> verts {
    vec3(v[0], 0), vec3(v[1], 0), vec3(v[2], 0),
    vec3(v[2], 0), vec3(v[1], 0), vec3(v[3], 0)
  };

  shader2_.BindBuffer(vbo, 0, 3);
  glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(glm::vec3), &verts[0]); 

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void WallPainting::DrawText(
  string text, vec2 point, vec3 color
) {
  Graphics::GetInstance().set_projection(glm::ortho(-texture_size_.x/2, texture_size_.x/2, texture_size_.y/2, -texture_size_.y/2));
  Graphics::GetInstance().DrawText(text, point.x, point.y, color);
  Graphics::GetInstance().set_projection();
}

void WallPainting::DrawCartesianGrid(
  int max_value, int tick_step, int big_tick_step
) {
  vec3 color = vec3(0.8, 0.8, 0.8);

  int size = 400;

  DrawLine(vec2(0, -size), vec2(0, size), 1, vec3(0));
  DrawLine(vec2(-size, 0), vec2(size, 0), 1, vec3(0));

  int step = (size / max_value);
  pixels_per_step_ = step;

  for (int y = -max_value; y <= max_value; y += tick_step) {
    DrawLine(vec2(y * step, -5), vec2(y * step, 4), 1, vec3(0));
    DrawLine(vec2(-5, y * step), vec2(4, y * step), 1, vec3(0));
  }

  for (int y = -max_value; y <= max_value; y += big_tick_step) {
    DrawLine(vec2(-size, y*step), vec2(size, y*step), 1, vec3(0.8));
    DrawLine(vec2(y*step, -size), vec2(y*step, size), 1, vec3(0.8));

    DrawLine(vec2(y*step, -10), vec2(y*step, 9), 1, vec3(0));
    DrawLine(vec2(-10, y*step), vec2(9, y*step), 1, vec3(0));

    if (y == 0) continue;
    stringstream ss;
    ss << y;
    DrawText(ss.str(), vec2(y * step -10, -30), vec3(0));
    DrawText(ss.str(), vec2(-40, y * step - 5), vec3(0));
  }
}

void WallPainting::BeginDraw() {
  glDisable(GL_CULL_FACE);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
  glViewport(0, 0, texture_size_.x, texture_size_.y);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader2_.program_id());

  glm::mat4 projection = glm::ortho(-texture_size_.x/2, texture_size_.x/2, texture_size_.y/2, -texture_size_.y/2);
  glUniformMatrix4fv(shader2_.GetUniformId("projection"), 1, GL_FALSE, &projection[0][0]);

  DrawCartesianGrid(6, 1, 2);
}

void WallPainting::EndDraw() {
  shader2_.Clear();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_CULL_FACE);
}

void WallPainting::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  shader_.BindTexture("TextureSampler", texture_);

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);

  shader_.Clear();
}

} // End of namespace.
