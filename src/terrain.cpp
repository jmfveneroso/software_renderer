#include "terrain.hpp"

namespace Sibyl {

Terrain::Terrain(
  std::shared_ptr<Player> player,
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id
) : player_(player),
    height_map_(std::make_shared<HeightMap>()),
    shader_(shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id) {

  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i] = Clipmap(player_, height_map_, i + 1); 
  }
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  // Textures.
  shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
  shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
  shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);

  // Clipmap.
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].Update(player_->position());
  }

  bool first = true;
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].Render(player_->position(), &shader_, ProjectionMatrix, ViewMatrix, first);
    first = false;
  }

  shader_.Clear();

  // static bool bla = false;
  // if(player_->position().x != 0 && !bla) {
  //   height_map_->CalculateErosion();
  //   bla = true;
  // }
}

void Terrain::Erode() {
  height_map_->CalculateErosion();
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].Clear();
  }
}

} // End of namespace.
