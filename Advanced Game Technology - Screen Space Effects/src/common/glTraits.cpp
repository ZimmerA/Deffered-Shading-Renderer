#include "glTraits.h"

ShaderProgramTraits::value_type ShaderProgramTraits::create() noexcept {
  return glCreateProgram();
}

void ShaderProgramTraits::destroy(
    ShaderProgramTraits::value_type handle) noexcept {
  glDeleteProgram(handle);
}

TextureTraits::value_type TextureTraits::create() noexcept {
  value_type handle;
  glGenTextures(1, &handle);
  return handle;
}

void TextureTraits::destroy(TextureTraits::value_type handle) noexcept {
  glDeleteTextures(1, &handle);
}

VertexArrayObjectTraits::value_type VertexArrayObjectTraits::create() noexcept {
  value_type handle;
  glGenVertexArrays(1, &handle);
  return handle;
}

void VertexArrayObjectTraits::destroy(
    VertexArrayObjectTraits::value_type handle) noexcept {
  glDeleteVertexArrays(1, &handle);
}

VertexBufferObjectTraits::value_type
VertexBufferObjectTraits::create() noexcept {
  value_type handle;
  glGenBuffers(1, &handle);
  return handle;
}

void VertexBufferObjectTraits::destroy(
    VertexBufferObjectTraits::value_type handle) noexcept {
  glDeleteBuffers(1, &handle);
}

FrameBufferTraits::value_type FrameBufferTraits::create() noexcept {
  value_type handle;
  glGenFramebuffers(1, &handle);
  return handle;
}

void FrameBufferTraits::destroy(FrameBufferTraits::value_type handle) noexcept {
  glDeleteFramebuffers(1, &handle);
}

RenderBufferTraits::value_type RenderBufferTraits::create() noexcept {
  value_type handle;
  glGenRenderbuffers(1, &handle);
  return handle;
}

void RenderBufferTraits::destroy(
    RenderBufferTraits::value_type handle) noexcept {
  glDeleteRenderbuffers(1, &handle);
}
