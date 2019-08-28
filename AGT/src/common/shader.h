#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>

#include "glTraits.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "openGLObjectBase.h"

class Shader {
public:
  Shader(const GLenum shaderType) noexcept;
  ~Shader() noexcept;
  void attachSource(const GLchar **source);
  void attachSourceFromFile(const char *path);

  GLuint handle;

private:
  GLenum type;
};

class ShaderProgram : public OpenGLObjectBase<ShaderProgramTraits> {
public:
  void attachShader(const Shader *shader) noexcept;
  void linkProgram();
  void use() noexcept;

  void setInt(const std::string &name, const int value) noexcept;
  void setFloat(const std::string &name, const float value) noexcept;
  void setVec3(const std::string &name, const glm::vec3 &value);
  void setVec3Arr(const std::string &name, const std::vector<glm::vec3> &value);
  void setVec2(const std::string &name, const glm::vec2 &value);
  void setMat4(const std::string &name, const glm::mat4 &value);
};

#endif // SHADER_H
