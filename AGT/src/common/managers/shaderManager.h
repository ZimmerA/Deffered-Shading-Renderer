#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "shader.h"

class ShaderManager {
public:
  std::shared_ptr<ShaderProgram> getProgramByName(const std::string &name);
  std::shared_ptr<ShaderProgram> createEmpty(const std::string &name);
  std::shared_ptr<ShaderProgram> create(const std::string &name,
                                        const std::string &vertexShaderPath,
                                        const std::string fragmentShaderPath);

private:
  std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> programs;
  bool exists(const std::string &name);
};

#endif // SHADER_MANAGER_H