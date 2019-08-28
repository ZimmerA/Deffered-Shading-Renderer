#include "material.h"

void PBRMaterial::prepare() {
  // TODO: Use data priented programming for materials instead of having it set
  // its own uniform values

  shader->setVec3("material.baseColorFactor", baseColorFactor);
  auto contains = textures.find(ALBEDO);
  if (contains != textures.end()) {
    shader->setInt("material.hasAlbedoMap", 1);

    glActiveTexture(GL_TEXTURE0);
    contains->second->bind();
  } else {
    shader->setInt("material.hasAlbedoMap", 0);
  }

  shader->setFloat("material.metallicFactor", metallicFactor);
  shader->setFloat("material.roughnessFactor", roughnessFactor);

  contains = textures.find(METALROUGHNESS);
  if (contains != textures.end()) {
    shader->setInt("material.hasMetalRoughnessMap", 1);
    glActiveTexture(GL_TEXTURE1);
    contains->second->bind();
  } else {
    shader->setInt("material.hasMetalRoughnessMap", 0);
  }

  contains = textures.find(NORMAL);
  if (contains != textures.end()) {
    shader->setInt("material.hasNormalMap", 1);
    glActiveTexture(GL_TEXTURE2);
    contains->second->bind();
  } else {
    shader->setInt("material.hasNormalMap", 0);
  }

  shader->setVec3("material.emissiveFactor", emissiveFactor);
  contains = textures.find(EMISSIVE);
  if (contains != textures.end()) {
    shader->setInt("material.hasEmissiveMap", 1);
    glActiveTexture(GL_TEXTURE3);
    contains->second->bind();
  } else {
    shader->setInt("material.hasEmissiveMap", 0);
  }

  contains = textures.find(OCCLUSION);
  if (contains != textures.end()) {
    shader->setInt("material.hasOcclusionMap", 1);
    glActiveTexture(GL_TEXTURE4);
    contains->second->bind();
  } else {
    shader->setInt("material.hasOcclusionMap", 0);
  }
}
