#include "renderer.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

Renderer::Renderer()  {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 
  glEnable(GL_CULL_FACE);

  // Why is this necessary? Should look on shaders. 
  // Vertex arrays group VBOs.
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  shader_ = Shader("text");
  CreateShaders();
  CreateVBOs();
  LoadFonts();
  LoadMeshes();
}

Renderer& Renderer::GetInstance() {
  static Renderer instance; 
  return instance;
}

void Renderer::CreateShaders() {
  shaders_["polygon"  ] = Shader("polygon", "v_lines", "f_lines"), 
  shaders_["building" ] = Shader("building");
  shaders_["object"   ] = Shader("object");
  shaders_["painting" ] = Shader("painting");
  shaders_["lines"    ] = Shader("lines");
  shaders_["intersect"] = Shader("intersect");
  shaders_["mask"     ] = Shader("mask");
  shaders_["screen"   ] = Shader("screen");
}

void Renderer::CreateVBOs() {
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

  vbos_["mask_uv"] = 0;
  glGenBuffers(1, &vbos_["mask_uv"]);
  vector<vec2> vertices { 
    vec2(0, 0),  vec2(1, 0),
    vec2(1, 1),  vec2(1, 1),
    vec2(0, 1),  vec2(0, 0)
  };
  glBindBuffer(GL_ARRAY_BUFFER, vbos_["mask_uv"]);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);

  vector<vec3> vertices2 {
    { -1, -1, 0.0 },
    { -1,  1, 0.0 },
    {  1, -1, 0.0 },
    {  1, -1, 0.0 },
    { -1,  1, 0.0 },
    {  1,  1, 0.0 }
  };

  vector<vec2> uvs = {
    { 0, 0 }, { 0, 1 },
    { 1, 0 }, { 1, 0 },
    { 0, 1 }, { 1, 1 }
  };

  vector<unsigned int> indices { 0, 1, 2, 3, 4, 5 };
  LoadMesh("screen", vertices2, uvs, indices);
}

void Renderer::LoadMesh(
  const string& name, 
  vector<vec3>& vertices, 
  vector<vec2>& uvs, 
  vector<unsigned int>& indices
) {
  Mesh m;
  glGenBuffers(1, &m.vertex_buffer_);
  glGenBuffers(1, &m.uv_buffer_);
  glGenBuffers(1, &m.element_buffer_);

  m.vertices_ = vertices;
  m.uvs_ = uvs;
  m.indices_ = indices;

  glBindBuffer(GL_ARRAY_BUFFER, m.vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, m.vertices_.size() * sizeof(glm::vec3), &m.vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, m.uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, m.uvs_.size() * sizeof(glm::vec2), &m.uvs_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.element_buffer_); glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    m.indices_.size() * sizeof(unsigned int), 
    &m.indices_[0], 
    GL_STATIC_DRAW
  );

  meshes_[name] = m;
}

void Renderer::LoadMesh(
  const string& name, 
  vector<vec3>& vertices, 
  vector<vec2>& uvs, 
  vector<vec3>& normals, 
  vector<unsigned int>& indices
) {
  Mesh m;
  glGenBuffers(1, &m.vertex_buffer_);
  glGenBuffers(1, &m.uv_buffer_);
  glGenBuffers(1, &m.normal_buffer_);
  glGenBuffers(1, &m.element_buffer_);

  m.vertices_ = vertices;
  m.uvs_ = uvs;
  m.normals_ = normals;
  m.indices_ = indices;

  glBindBuffer(GL_ARRAY_BUFFER, m.vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, m.vertices_.size() * sizeof(glm::vec3), &m.vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, m.uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, m.uvs_.size() * sizeof(glm::vec2), &m.uvs_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, m.normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, m.normals_.size() * sizeof(glm::vec3), &m.normals_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.element_buffer_); glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    m.indices_.size() * sizeof(unsigned int), 
    &m.indices_[0], 
    GL_STATIC_DRAW
  );

  meshes_[name] = m;
}

void Renderer::LoadMesh(const string& name) {
  string filename = "meshes/" + name + ".obj";
  ifstream f(filename);
  if (!f.is_open()) return;

  vector<glm::vec3> vertex_lookup;
  vector<glm::vec2> uv_lookup;
  vector<glm::vec3> normal_lookup;

  vector<glm::vec3> vertices;
  vector<glm::vec2> uvs;
  vector<glm::vec3> normals;
  vector<unsigned int> indices;

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
        vertices.push_back(vertex_lookup[vertex_ids[i]]);
        uvs.push_back(uv_lookup[uv_ids[i]]);
        normals.push_back(vertex_lookup[normal_ids[i]]);
        indices.push_back(vertices.size()-1);
      }
    }
  }
  f.close();

  LoadMesh(name, vertices, uvs, normals, indices);
}

void Renderer::CreateFramebuffer(const string& name, int width, int height) {
  FBO fbo;
  fbo.width = width;
  fbo.height = height;

  glGenTextures(1, &fbo.texture);
  glBindTexture(GL_TEXTURE_2D, fbo.texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fbo.width, fbo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenRenderbuffers(1, &fbo.depth_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, fbo.depth_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbo.width, fbo.height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glGenFramebuffers(1, &fbo.framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo.framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo.depth_rbo);
  
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw;

  fbos_[name] = fbo;
}

void Renderer::LoadMeshes() {
  LoadMesh("book_stand");
  LoadMesh("sculpture");
  LoadMesh("scroll");

  float w = 2.0f;
  float h = 2.0f;
  float t = 0.1;

  vector<vec3> v {
    // Back face.
    vec3(-w/2, h/2, -t/2), vec3(w/2, h/2, -t/2), vec3(-w/2, -h/2, -t/2), vec3(w/2, -h/2, -t/2),
    // Front face.
    vec3(-w/2, h/2, t/2), vec3(w/2, h/2, t/2), vec3(-w/2, -h/2, t/2), vec3(w/2, -h/2, t/2),
  };

  vector<vec3> vertices_ = {
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

  vector<unsigned int> indices_;
  for (int i = 0; i < 36; i++) indices_.push_back(i);
  LoadMesh("2d_plot", vertices_, uvs, indices_);
}

void Renderer::LoadFonts() {
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

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (c == 150) {
      unsigned char buffer[14 * 7];
      for (int y = 0; y < 14; y++)
        for (int x = 0; x < 7; x++)
          buffer[y * 7 + x] = 255;

      face->glyph->bitmap_top = 12;
      glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        7,
        14,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        &buffer
      );
    } else {
      // Generate texture
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
    }

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

  glGenBuffers(1, &text_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

  projection_ = glm::ortho(0.0f, (float) WINDOW_WIDTH, 0.0f, (float) WINDOW_HEIGHT); 
}

void Renderer::DrawChar(char c, float x, float y, vec3 color, GLfloat scale) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glUseProgram(shader_.program_id());

  // Activate corresponding render state	
  glUniform3f(glGetUniformLocation(shader_.program_id(), "textColor"), color.x, color.y, color.z);
  glUniformMatrix4fv(shader_.GetUniformId("projection"), 1, GL_FALSE, &projection_[0][0]);
  glActiveTexture(GL_TEXTURE0);

  Character& ch = characters_[c];

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
  shader_.BindBuffer(text_vbo_, 0, 4);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

  // Render quad
  glDrawArrays(GL_TRIANGLES, 0, 6);

  shader_.Clear();
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawText(const string& text, float x, float y, vec3 color, GLfloat scale) {
  // Iterate through all characters
  for (const auto& c : text) {
    DrawChar(c, x, y, color, scale);

    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (characters_[c].Advance >> 6) * scale;
  }
}

void Renderer::DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, vec3 color) {
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glUseProgram(shaders_["polygon"].program_id());

  vector<vec3> vertices = {
    { x        , y         , 0.0 },
    { x        , y - height, 0.0 },
    { x + width, y         , 0.0 },
    { x + width, y         , 0.0 },
    { x        , y - height, 0.0 },
    { x + width, y - height, 0.0 }
  };

  glUniform3f(shaders_["polygon"].GetUniformId("lineColor"), color.x, color.y, color.z);
  glUniformMatrix4fv(shaders_["polygon"].GetUniformId("projection"), 1, GL_FALSE, &projection_[0][0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]); 

  shaders_["polygon"].BindBuffer(vbo_, 0, 3);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  shaders_["polygon"].Clear();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void Renderer::DrawMesh(
  string mesh_name, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, 
  glm::vec3 camera, glm::vec3 position, GLfloat rotation, bool highlighted
) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  Mesh& mesh = meshes_[mesh_name];
  glUseProgram(shaders_["object"].program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position);
  ModelMatrix *= glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;

  glUniformMatrix4fv(shaders_["object"].GetUniformId("MVP"), 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shaders_["object"].GetUniformId("M"), 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shaders_["object"].GetUniformId("V"), 1, GL_FALSE, &ViewMatrix[0][0]);

  if (highlighted) {
    glUniform1f(shaders_["object"].GetUniformId("highlight"), 1.0);
  } else {
    glUniform1f(shaders_["object"].GetUniformId("highlight"), 0.0);
  }

  shaders_["object"].BindBuffer(mesh.vertex_buffer_, 0, 3);
  shaders_["object"].BindBuffer(mesh.uv_buffer_, 1, 2);
  shaders_["object"].BindBuffer(mesh.normal_buffer_, 2, 3);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.element_buffer_);
  glDrawElements(GL_TRIANGLES, mesh.indices_.size(), GL_UNSIGNED_INT, (void*) 0);

  shaders_["object"].Clear();
  // meshes_[mesh_name].Draw(ProjectionMatrix, ViewMatrix, camera, position, rotation, highlighted);
}

void Renderer::DrawCube(
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

void Renderer::DrawLine(
  vec2 p1, vec2 p2, GLfloat thickness, vec3 color
) {
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glUseProgram(shaders_["polygon"].program_id());
  GLfloat s = thickness / 2.0f;
  vec2 step = normalize(p2 - p1);

  vector<vec2> v {
    p1 + s * (vec2(-step.y, step.x)), p1 + s * (vec2(step.y, -step.x)),
    p2 + s * (vec2(-step.y, step.x)), p2 + s * (vec2(step.y, -step.x))
  };

  glUniform3f(shaders_["polygon"].GetUniformId("lineColor"), color.x, color.y, color.z);
  glUniformMatrix4fv(shaders_["polygon"].GetUniformId("projection"), 1, GL_FALSE, &projection_[0][0]);

  std::vector<glm::vec3> lines = {
    vec3(v[0], 0), vec3(v[1], 0), vec3(v[2], 0), vec3(v[2], 0), vec3(v[1], 0), vec3(v[3], 0)
  };

  shaders_["polygon"].BindBuffer(vbo_, 0, 3);
  glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(glm::vec3), &lines[0]); 

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  shaders_["polygon"].Clear();
}

void Renderer::DrawPoint(
  vec2 point, GLfloat thickness, vec3 color
) {
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glUseProgram(shaders_["polygon"].program_id());
  set_projection(glm::ortho(-512, 512, 512, -512));
  GLfloat s = thickness / 2.0f;
  vector<vec2> v {
    point + vec2(-s, -s), point + vec2(-s, s), 
    point + vec2(s, -s), point + vec2(s, s)
  };

  glUniform3f(shaders_["polygon"].GetUniformId("lineColor"), color.x, color.y, color.z);

  std::vector<glm::vec3> verts {
    vec3(v[0], 0), vec3(v[1], 0), vec3(v[2], 0),
    vec3(v[2], 0), vec3(v[1], 0), vec3(v[3], 0)
  };

  shaders_["polygon"].BindBuffer(vbo_, 0, 3);
  glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(glm::vec3), &verts[0]); 

  glDrawArrays(GL_TRIANGLES, 0, 6);
  set_projection();
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  shaders_["polygon"].Clear();
}

void Renderer::DrawArrow(
  vec2 p1, vec2 p2, GLfloat thickness, vec3 color
) {
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  GLfloat height = 12.0f;
  GLfloat width = 5.0f;
  GLfloat steepness = 0.75f;
  glUseProgram(shaders_["polygon"].program_id());

  vec2 step = normalize(p2 - p1);
  DrawLine(p1, p2 - step * (height * steepness), thickness, color);

  vector<vec2> v {
    p2, 
    (p2 - step * (height * steepness)),
    (p2 - step * height) + (width * vec2(-step.y, step.x)),
    (p2 - step * height) + (width * vec2(step.y, -step.x))
  };

  glUniform3f(shaders_["polygon"].GetUniformId("lineColor"), color.x, color.y, color.z);
  glUniformMatrix4fv(shaders_["polygon"].GetUniformId("projection"), 1, GL_FALSE, &projection_[0][0]);

  std::vector<glm::vec3> lines = {
    vec3(v[0], 0), vec3(v[1], 0), vec3(v[2], 0),
    vec3(v[0], 0), vec3(v[1], 0), vec3(v[3], 0)
  };

  shaders_["polygon"].BindBuffer(vbo_, 0, 3);
  glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(glm::vec3), &lines[0]); 

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  shaders_["polygon"].Clear();
}

void Renderer::DrawOneDimensionalSpace(
  int max_value, int tick_step, int big_tick_step
) {
  vec3 color = vec3(0.8, 0.8, 0.8);

  int size = 400;

  DrawLine(vec2(0, -size), vec2(0, size), 1, vec3(0));
  DrawLine(vec2(-size, 0), vec2(size, 0), 1, vec3(0));

  int step = (size / max_value);
  for (int y = -max_value; y <= max_value; y += tick_step) {
    DrawLine(vec2(y * step, -5), vec2(y * step, 4), 1, vec3(0));
  }

  for (int y = -max_value; y <= max_value; y += big_tick_step) {
    DrawLine(vec2(y*step, -10), vec2(y*step, 9), 1, vec3(0));

    stringstream ss;
    ss << y;
    DrawText(ss.str(), y * step -10, -30, vec3(0));
  }
}

void Renderer::DrawCartesianGrid(
  int max_value, int tick_step, int big_tick_step
) {
  set_projection(glm::ortho(-512, 512, 512, -512));
  vec3 color = vec3(0.8, 0.8, 0.8);

  int size = 400;

  DrawLine(vec2(0, -size), vec2(0, size), 1, vec3(0));
  DrawLine(vec2(-size, 0), vec2(size, 0), 1, vec3(0));
  int step = (size / max_value);
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
    DrawText(ss.str(), y * step -10, -30, vec3(0));
    DrawText(ss.str(), -40, y * step - 5, vec3(0));
  }
  set_projection();
}

void Renderer::DrawHighlightedObject(
  string mesh_name, 
  mat4 ProjectionMatrix, 
  mat4 ViewMatrix, 
  vec3 camera, 
  vec3 position,
  GLfloat rotation,
  bool highlighted,
  GLuint main_texture,
  GLfloat alpha 
) {
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;

  Mesh& m = meshes_[mesh_name];
  if (highlighted) {
    // Draw mask.
    glBindFramebuffer(GL_FRAMEBUFFER, fbos_["intersect"].framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glUseProgram(shaders_["intersect"].program_id());
    glUniformMatrix4fv(shaders_["intersect"].GetUniformId("MVP"), 1, GL_FALSE, &MVP[0][0]);
    shaders_["intersect"].BindBuffer(m.vertex_buffer_, 0, 3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.element_buffer_);
    glDrawElements(GL_TRIANGLES, m.indices_.size(), GL_UNSIGNED_INT, (void*) 0);
    shaders_["intersect"].Clear();

    // Draw outline.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, fbos_["screen"].framebuffer);
    glUseProgram(shaders_["mask"].program_id());

    vec2 pixel_size(1.0 / 1200, 1.0 / 800);
    glUniform2f(shaders_["mask"].GetUniformId("pixel_size"), pixel_size.x, pixel_size.y);
    glUniform3f(shaders_["mask"].GetUniformId("outline_color"), 1.0, 0.69, 0.23);
  }

  shaders_["mask"].BindBuffer(vbos_["mask_uv"], 0, 2);
  shaders_["mask"].BindTexture("TextureSampler", fbos_["intersect"].texture);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  shaders_["mask"].Clear();
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(shaders_["painting"].program_id());
  glUniformMatrix4fv(shaders_["painting"].GetUniformId("MVP"), 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shaders_["painting"].GetUniformId("M"), 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniform1f(shaders_["painting"].GetUniformId("alpha"), alpha);
  shaders_["painting"].BindTexture("TextureSampler", main_texture);
  shaders_["painting"].BindBuffer(m.vertex_buffer_, 0, 3);
  shaders_["painting"].BindBuffer(m.uv_buffer_, 1, 2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.element_buffer_);
  glDrawElements(GL_TRIANGLES, m.indices_.size(), GL_UNSIGNED_INT, (void*) 0);
  shaders_["painting"].Clear();
  glDisable(GL_BLEND);
}

void Renderer::DrawScreen(bool blur) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, fbos_["screen"].width, fbos_["screen"].height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Mesh& m = meshes_["screen"];

  glDisable(GL_CULL_FACE);
  glUseProgram(shaders_["screen"].program_id());
  glUniform1f(glGetUniformLocation(shaders_["screen"].program_id(), "blur"), (blur) ? 1.0 : 0.0);
  shaders_["screen"].BindTexture("TextureSampler", fbos_["screen"].texture);
  shaders_["screen"].BindBuffer(m.vertex_buffer_, 0, 3);
  shaders_["screen"].BindBuffer(m.uv_buffer_, 1, 2);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  shaders_["screen"].Clear();
  glEnable(GL_CULL_FACE);
}

} // End of namespace.
