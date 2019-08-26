#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "material.h"

class MaterialManager {
public:
  std::shared_ptr<PBRMaterial> getMaterialByName(const std::string &name);
  std::shared_ptr<PBRMaterial> createEmpty(const std::string &name);

private:
  std::unordered_map<std::string, std::shared_ptr<PBRMaterial>> materials;
};
#endif // MATERIALMANAGER_H
