#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "texture.h"

class TextureManager {
public:
  std::shared_ptr<Texture> getTextureByName(const std::string &name);
  std::shared_ptr<Texture> create(const std::string &name, unsigned char *data,
                                  const int width, const int height,
                                  const int componentCount);
  std::shared_ptr<Texture> createEmpty(const std::string &name);

private:
  std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};

#endif // TEXTUREMANAGER_H