#include <algorithm>
#include <iostream>
#include <stdlib.h>

#include "glm/glm/gtc/type_ptr.hpp"
#include "shader.h"
#include "util.h"

/*
 * Shader
 */
Shader::Shader(const GLenum shaderType) noexcept {
  handle = glCreateShader(shaderType);
  type = shaderType;
}

Shader::~Shader() noexcept { glDeleteShader(handle); }

void Shader::attachSource(const GLchar **source) {
  glShaderSource(handle, 1, source, NULL);
  glCompileShader(handle);

  int success;
  char infoLog[512];

  glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(handle, 512, nullptr, infoLog);
    std::string shader;
    switch (type) {
    case GL_VERTEX_SHADER:
      shader = "VERTEX";
      break;
    case GL_FRAGMENT_SHADER:
      shader = "FRAGMENT";
      break;
    case GL_GEOMETRY_SHADER:
      shader = "GEOMETRY";
      break;
    case GL_TESS_CONTROL_SHADER:
      shader = "TESS_CONTROL";
      break;
    case GL_TESS_EVALUATION_SHADER:
      shader = "TESS_EVALUATION";
      break;
    case GL_COMPUTE_SHADER:
      shader = "COMPUTE";
      break;
    }
    std::cout << "ERROR::SHADER::" << shader << "::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
}

void Shader::attachSourceFromFile(const char *path) {
  const char *data = loadFile(path);
  attachSource(&data);
  free((void *)data);
}

/*
 * ShaderProgram
 */
void ShaderProgram::attachShader(const Shader *shader) noexcept {
  glAttachShader(handle, shader->handle);
}

void ShaderProgram::linkProgram() {
  glLinkProgram(handle);

  int success;
  char infoLog[512];

  glGetProgramiv(handle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(handle, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
}

void ShaderProgram::use() noexcept { glUseProgram(handle); }

void ShaderProgram::setInt(const std::string &name, const int value) noexcept {
  glUniform1i(glGetUniformLocation(handle, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string &name,
                             const float value) noexcept {
  glUniform1f(glGetUniformLocation(handle, name.c_str()), value);
}

void ShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) {
  glUniform3fv(glGetUniformLocation(handle, name.c_str()), 1,
               glm::value_ptr(value));
}

void ShaderProgram::setVec3Arr(const std::string &name,
                               const std::vector<glm::vec3> &value) {
  glUniform3fv(glGetUniformLocation(handle, name.c_str()), static_cast<GLsizei>(value.size()),
               glm::value_ptr(value[0]));
}

void ShaderProgram::setVec2(const std::string &name, const glm::vec2 &value) {
  glUniform2fv(glGetUniformLocation(handle, name.c_str()), 1,
               glm::value_ptr(value));
}

void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &value) {
  glUniformMatrix4fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}
