#include "materialManager.h"

std::shared_ptr<PBRMaterial>
MaterialManager::getMaterialByName(const std::string &name) {
  return materials[name.c_str()];
}

std::shared_ptr<PBRMaterial>
MaterialManager::createEmpty(const std::string &name) {
  if (materials.find(name.c_str()) != materials.end()) {
    printf("WARNING, MATERIAL WITH NAME: %s ALREADY EXISTS", name.c_str());
    return nullptr;
  }

  std::shared_ptr<PBRMaterial> material = std::make_shared<PBRMaterial>();

  materials[name.c_str()] = material;
  return material;
}
