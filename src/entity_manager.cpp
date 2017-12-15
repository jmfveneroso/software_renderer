#include "entity_manager.hpp"

namespace Sibyl {

EntityManager::EntityManager() {
  Initialize();
}

void EntityManager::LoadTexture(
  const std::string& name, 
  const std::string& texture_file_path
) {
  GLuint texture_id = loadBMP_custom(texture_file_path.c_str());
  textures_.insert({ name, texture_id });
}

void EntityManager::CreateFrameBuffer(
  const std::string& name, int width, 
  int height, glm::vec2 top_left
) {
  FrameBuffer fb = FrameBuffer(width, height, top_left);
  frame_buffers_.insert({ name, fb });
  textures_.insert({ name + ".texture", fb.GetTexture() });
  textures_.insert({ name + ".depth_texture", fb.GetDepthTexture() });
}

void EntityManager::LoadShader(
  const std::string& name,  
  const std::string& vertex_file_path, 
  const std::string& fragment_file_path
) {
  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if (VertexShaderStream.is_open()){
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  } else {
    printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
    getchar();
    return;
  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if (FragmentShaderStream.is_open()) {
    std::string Line = "";
    while (getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int InfoLogLength;

  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path.c_str());
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path.c_str());
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }

  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);
  
  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  shaders_.insert({ name, ProgramID });
}

void EntityManager::LoadEntity(
  const std::string& name, 
  const std::string& obj_file_path,
  const std::string& texture_file_path,
  const std::string& normals_file_path,
  const std::string& specular_file_path,
  const std::string& shader
) {
  GLuint shader_id = shaders_[shader];
  RenderObject obj = RenderObject(obj_file_path, texture_file_path, normals_file_path, specular_file_path, shader_id, false);
  entities_.insert({ name, obj });
}

void EntityManager::Initialize() {
  CreateFrameBuffer("reflection", 1000, 750, vec2( 0.0f,  0.0f));
  CreateFrameBuffer("refraction", 1000, 750, vec2(-1.0f,  0.0f));
  CreateFrameBuffer("screen",     1000, 750, vec2(-1.0f, -1.0f));

  // LoadTexture("default", "shaders/vshade_normals", "shaders/fshade_normals");

  // water = WaterRenderObject("res/water2.obj", reflection_water.GetTexture(), refraction_water.GetTexture(), "textures/water_dudv.bmp", "textures/water_normal.bmp", waterProgramID, refraction_water.GetDepthTexture());

  // Create and compile our GLSL program from the shaders.
  LoadShader("default", "shaders/vshade_normals", "shaders/fshade_normals");
  LoadShader("sky", "shaders/vshade_normals", "shaders/fshade_sky");
  LoadShader("water", "shaders/vshade_water", "shaders/fshade_water");

  LoadEntity(
    "terrain", "res/large_terrain.obj", "textures/large_terrain.bmp", 
    "textures/normal.bmp", "textures/specular_orange.bmp", "default"
  );

  LoadEntity(
    "sky", "res/skydome2.obj", "textures/skydome.bmp", 
    "textures/normal.bmp", "textures/specular_orange.bmp", "sky"
  );

  // LoadEntity(
  //   "water", "res/water2.obj", reflection_water.GetTexture(),
  //   refraction_water.GetTexture(), "textures/water_dudv.bmp", "water",
  //   refraction_water.GetDepthTexture()
  // );
}

RenderObject EntityManager::GetEntity(const std::string& name) {
  return entities_[name];
}

void EntityManager::Clean() {
  // for (auto shader_id : shaders_)
  //   glDeleteProgram(shaders_[shader_id]);

  // for (auto entity : entities_)
  //   entities_[entity].Clean();
}

} // End of namespace.
