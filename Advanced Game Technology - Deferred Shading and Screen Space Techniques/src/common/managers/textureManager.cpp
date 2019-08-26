#include "textureManager.h"

std::shared_ptr<Texture>
TextureManager::getTextureByName(const std::string &name) {
  return textures[name.c_str()];
}

std::shared_ptr<Texture> TextureManager::create(const std::string &name,
                                                unsigned char *data,
                                                const int width,
                                                const int height,
                                                const int componentCount) {

  if (textures.find(name.c_str()) != textures.end()) {
    printf("WARNING, TEXTURE WITH NAME: %s ALREADY EXISTS", name.c_str());
    return nullptr;
  }
  const GLenum pixelFormat = [&] {
    if (componentCount == 3)
      return GL_RGB;
    else
      return GL_RGBA;
  }();

  std::shared_ptr<Texture> texture = std::make_shared<Texture>();
  texture->texImage(0, pixelFormat, width, height, 0, pixelFormat,
                    GL_UNSIGNED_BYTE, data);
  texture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  texture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture->bind();
  glGenerateMipmap(GL_TEXTURE_2D);
  textures[name.c_str()] = texture;
  return texture;
}

std::shared_ptr<Texture> TextureManager::createEmpty(const std::string &name) {
  if (textures.find(name.c_str()) != textures.end()) {
    printf("WARNING, TEXTURE WITH NAME: %s ALREADY EXISTS", name.c_str());
    return nullptr;
  }
  std::shared_ptr<Texture> texture = std::make_shared<Texture>();
  textures[name.c_str()] = texture;
  return texture;
}
