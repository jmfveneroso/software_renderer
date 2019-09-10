#include "graphics.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

Mesh::Mesh(
  const string& filename
) : shader_("object") {
  Load(filename);
}

void Mesh::Load(const string& filename) {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &normal_buffer_);
  glGenBuffers(1, &element_buffer_);

  ifstream f(filename);
  if (!f.is_open()) return;

  vector<glm::vec3> vertex_lookup;
  vector<glm::vec2> uv_lookup;
  vector<glm::vec3> normal_lookup;
  vector<glm::vec2> uvs;

  string line;
  while (getline(f, line)) {
    vector<string> tokens;
    boost::split(tokens, line, boost::is_any_of(" "));
    if (!tokens.size()) continue;

    string type = tokens[0];
    if (type == "v") {
      glm::vec3 vertex;
      vertex.x = boost::lexical_cast<float>(tokens[1]); 
      vertex.y = boost::lexical_cast<float>(tokens[2]);
      vertex.z = boost::lexical_cast<float>(tokens[3]);
      vertex_lookup.push_back(vertex);
    } else if (type == "vt") {
      glm::vec2 uv_coordinate;
      uv_coordinate.x = boost::lexical_cast<float>(tokens[1]); 
      uv_coordinate.y = boost::lexical_cast<float>(tokens[2]);
      uv_lookup.push_back(uv_coordinate);
    } else if (type == "vn") {
      glm::vec3 normal;
      normal.x = boost::lexical_cast<float>(tokens[1]); 
      normal.y = boost::lexical_cast<float>(tokens[2]);
      normal.z = boost::lexical_cast<float>(tokens[3]);
      normal_lookup.push_back(normal);
    } else if (type == "f") {
      vector<unsigned int> vertex_ids;
      vector<unsigned int> uv_ids;
      vector<unsigned int> normal_ids;
      for (int i = 1; i < tokens.size(); i++) {
        string& s = tokens[i];
        size_t j = s.find_first_of("/", 0);
        vertex_ids.push_back(boost::lexical_cast<unsigned int>(s.substr(0, j)) - 1); 

        size_t k = s.find_first_of("/", j+1);
        uv_ids.push_back(boost::lexical_cast<unsigned int>(s.substr(j+1, k-j-1)) - 1); 

        normal_ids.push_back(boost::lexical_cast<unsigned int>(s.substr(k+1)) - 1); 
      }
  
      for (int i = 0; i < 3; i++) { 
        vertices_.push_back(vertex_lookup[vertex_ids[i]]);
        uvs.push_back(uv_lookup[uv_ids[i]]);
        normals_.push_back(vertex_lookup[normal_ids[i]]);
        indices_.push_back(vertices_.size()-1);
      }
    }
  }
  f.close();

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), &normals_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_); glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );
}

void Mesh::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera, glm::vec3 position, GLfloat rotation) {
  glUseProgram(shader_.program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position);
  ModelMatrix *= glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"), 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"), 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"), 1, GL_FALSE, &ViewMatrix[0][0]);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  shader_.BindBuffer(normal_buffer_, 2, 3);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);

  shader_.Clear();
}

Graphics::Graphics() : shader_("text") {
  CreateShaders();
  CreateVBOs();
  LoadFonts();
  LoadMeshes();
}

Graphics& Graphics::GetInstance() {
  static Graphics instance; 
  return instance;
}

void Graphics::CreateShaders() {
  shaders_["polygon"] = Shader("terminal", "v_terminal", "f_terminal");
  shaders_["building"] = Shader("building");
}

void Graphics::CreateVBOs() {
  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

  vbos_["cube_v"] = 0;
  glGenBuffers(1, &vbos_["cube_v"]);
  glBindBuffer(GL_ARRAY_BUFFER, vbos_["cube_v"]);
  glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

  vbos_["cube_uv"] = 0;
  glGenBuffers(1, &vbos_["cube_uv"]);
  glBindBuffer(GL_ARRAY_BUFFER, vbos_["cube_uv"]);
  glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
}

void Graphics::LoadMeshes() {
  meshes_["book_stand"] = Mesh("meshes/book_stand.obj");
  meshes_["scroll"] = Mesh("meshes/scroll.obj");
}

void Graphics::LoadFonts() {
  FT_Library ft;
  if (FT_Init_FreeType(&ft))
    cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
  
  FT_Face face;
  if (FT_New_Face(ft, "fonts/ubuntu_monospace.ttf", 0, &face))
    cout << "ERROR::FREETYPE: Failed to load font" << endl; 

  if (FT_Set_Char_Size(face, 0, 16*64, 300, 300))
    cout << "ERROR::FREETYPE: Failed to set char size" << endl; 
   
  FT_Set_Pixel_Sizes(face, 0, 18);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (GLubyte c = 0; c < 255; c++) {
    // Load character glyph 
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        continue;
    }

    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Now store character for later use
    characters_[c] = {
      texture, 
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      (GLuint) face->glyph->advance.x
    };
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

  projection_ = glm::ortho(0.0f, (float) WINDOW_WIDTH, 0.0f, (float) WINDOW_HEIGHT); 
}

void Graphics::DrawChar(char c, float x, float y, vec3 color) {
  Character& ch = characters_[c];

  GLfloat scale = 1.0f;
  GLfloat xpos = x + ch.Bearing.x * scale;
  GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

  GLfloat w = ch.Size.x * scale;
  GLfloat h = ch.Size.y * scale;

  // Update VBO for each character
  GLfloat vertices[6][4] = {
    { xpos,     ypos + h,   0.0, 0.0 },            
    { xpos,     ypos,       0.0, 1.0 },
    { xpos + w, ypos,       1.0, 1.0 },

    { xpos,     ypos + h,   0.0, 0.0 },
    { xpos + w, ypos,       1.0, 1.0 },
    { xpos + w, ypos + h,   1.0, 0.0 }           
  };

  // Render glyph texture over quad
  glBindTexture(GL_TEXTURE_2D, ch.TextureID);

  // Update content of VBO memory
  shader_.BindBuffer(vbo_, 0, 4);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

  // Render quad
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Graphics::DrawText(const string& text, float x, float y, vec3 color) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(shader_.program_id());

  // Activate corresponding render state	
  glUniform3f(glGetUniformLocation(shader_.program_id(), "textColor"), color.x, color.y, color.z);
  glUniformMatrix4fv(shader_.GetUniformId("projection"), 1, GL_FALSE, &projection_[0][0]);
  glActiveTexture(GL_TEXTURE0);

  // Iterate through all characters
  for (const auto& c : text) {
    DrawChar(c, x, y, color);

    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    float scale = 1.0;
    x += (characters_[c].Advance >> 6) * scale;
  }

  shader_.Clear();
  glDisable(GL_BLEND);
}

void Graphics::Rectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, vec3 color) {
  glUseProgram(shaders_["polygon"].program_id());

  vector<vec3> vertices = {
    { x        , y         , 0 },
    { x        , y - height, 0 },
    { x + width, y         , 0 },
    { x + width, y         , 0 },
    { x        , y - height, 0 },
    { x + width, y - height, 0 }
  };

  glUniformMatrix4fv(shaders_["polygon"].GetUniformId("projection"), 1, GL_FALSE, &projection_[0][0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]); 

  shaders_["polygon"].BindBuffer(vbo_, 0, 3);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  shaders_["polygon"].Clear();
}

void Graphics::DrawMesh(
  string mesh_name, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera,
  glm::vec3 position, GLfloat rotation
) {
  meshes_[mesh_name].Draw(ProjectionMatrix, ViewMatrix, camera, position, rotation);
}

void Graphics::Cube(
  glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera,
  glm::vec3 position, vec3 dimensions, GLfloat rotation
) {
  float w = dimensions.x;
  float l = dimensions.y;
  float h = dimensions.z;
 
  vector<vec3> v {
    // Back face.
    vec3(0, h, 0), vec3(w, h, 0), vec3(0, 0, 0), vec3(w, 0, 0),
    // Front face.
    vec3(0, h, l), vec3(w, h, l), vec3(0, 0, l), vec3(w, 0, l),
  };

  vector<vec3> vertices = {
    v[0], v[4], v[1], v[1], v[4], v[5], // Top.
    v[1], v[3], v[0], v[0], v[3], v[2], // Back.
    v[0], v[2], v[4], v[4], v[2], v[6], // Left.
    v[5], v[7], v[1], v[1], v[7], v[3], // Right.
    v[4], v[6], v[5], v[5], v[6], v[7], // Front.
    v[6], v[2], v[7], v[7], v[2], v[3]  // Bottom.
  };

  vector<vec2> u = {
    vec2(0, 0), vec2(0, l), vec2(w, 0), vec2(w, l), // Top.
    vec2(0, 0), vec2(0, h), vec2(w, 0), vec2(w, h), // Back.
    vec2(0, 0), vec2(0, h), vec2(l, 0), vec2(l, h)  // Left.
  };

  vector<glm::vec2> uvs {
    u[0], u[1], u[2],  u[2],  u[1], u[3],  // Top.
    u[4], u[5], u[6],  u[6],  u[5], u[7],  // Back.
    u[8], u[9], u[10], u[10], u[9], u[11], // Left.
    u[8], u[9], u[10], u[10], u[9], u[11], // Right.
    u[4], u[5], u[6],  u[6],  u[5], u[7],  // Front.
    u[0], u[1], u[2],  u[2],  u[1], u[3]   // Bottom.
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbos_["cube_v"]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]); 

  glBindBuffer(GL_ARRAY_BUFFER, vbos_["cube_uv"]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, uvs.size() * sizeof(glm::vec2), &uvs[0]); 

  glUseProgram(shaders_["building"].program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;

  glUniformMatrix4fv(shaders_["building"].GetUniformId("MVP"), 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shaders_["building"].GetUniformId("M"), 1, GL_FALSE, &ModelMatrix[0][0]);

  shaders_["building"].BindBuffer(vbos_["cube_v"], 0, 3);
  shaders_["building"].BindBuffer(vbos_["cube_uv"], 1, 2);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  shaders_["building"].Clear();
}

} // End of namespace.
