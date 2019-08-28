#include "shaderManager.h"
#include "shader.h"

std::shared_ptr<ShaderProgram>
ShaderManager::getProgramByName(const std::string &name) {
  return programs[name.c_str()];
}

std::shared_ptr<ShaderProgram>
ShaderManager::createEmpty(const std::string &name) {
  if (exists(name)) {
    printf("WARNING, SHADERPROGRAM WITH NAME: %s ALREADY EXISTS", name.c_str());
    return nullptr;
  }

  std::shared_ptr<ShaderProgram> program = std::make_shared<ShaderProgram>();
  programs[name.c_str()] = program;
  return program;
}

std::shared_ptr<ShaderProgram>
ShaderManager::create(const std::string &name,
                      const std::string &vertexShaderPath,
                      const std::string fragmentShaderPath) {

  std::shared_ptr<ShaderProgram> program = std::make_shared<ShaderProgram>();

  Shader vs(GL_VERTEX_SHADER);
  vs.attachSourceFromFile(vertexShaderPath.c_str());
  Shader fs(GL_FRAGMENT_SHADER);
  fs.attachSourceFromFile(fragmentShaderPath.c_str());

  program->attachShader(&vs);
  program->attachShader(&fs);
  program->linkProgram();

  programs[name.c_str()] = program;
  return program;
}

bool ShaderManager::exists(const std::string &name) {
  return (programs.find(name.c_str()) != programs.end());
}
