#include "texture.h"

void Texture::bind() noexcept { glBindTexture(GL_TEXTURE_2D, handle); }

void Texture::unbind() noexcept { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::texImage(GLint level, GLint internalFormat, GLsizei width,
                       GLsizei height, GLint border, GLenum format, GLenum type,
                       const GLvoid *data) noexcept {
  bind();
  glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border,
               format, type, data);
  unbind();
}

void Texture::texParameteri(GLenum name, GLint value) noexcept {
  bind();
  glTexParameteri(GL_TEXTURE_2D, name, value);
  unbind();
}
