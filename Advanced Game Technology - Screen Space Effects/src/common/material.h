#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"
#include "shader.h"
#include "texture.h"

enum AlphaMode { M_OPAQUE, M_MASK, M_BLEND };

class PBRMaterial {
public:
  enum TextureType { ALBEDO, NORMAL, METALROUGHNESS, EMISSIVE, OCCLUSION };

  PBRMaterial(std::shared_ptr<ShaderProgram> shader = nullptr,
              glm::vec3 baseColorFactor = glm::vec3{1},
              float metallicFactor = 1.f, float roughnessFactor = 1.f,
              glm::vec3 emissiveFactor = glm::vec3{0})
      : shader{shader}, baseColorFactor{baseColorFactor},
        metallicFactor{metallicFactor}, roughnessFactor{roughnessFactor},
        emissiveFactor{emissiveFactor} {};

  void prepare();

  std::shared_ptr<ShaderProgram> shader;
  std::unordered_map<TextureType, std::shared_ptr<Texture>> textures;
  glm::vec3 baseColorFactor;
  float metallicFactor;
  float roughnessFactor;
  glm::vec3 emissiveFactor;
  AlphaMode mode = M_OPAQUE;
};

#endif // MATERIAL_H