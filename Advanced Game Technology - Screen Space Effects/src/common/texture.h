#ifndef TEXTURE_H
#define TEXTURE_H

#include "glTraits.h"
#include "glad.h"
#include "openGLObjectBase.h"

class Texture : public OpenGLObjectBase<TextureTraits> {
public:
  void bind() noexcept;
  void unbind() noexcept;
  void texImage(GLint level, GLint internalFormat, GLsizei width,
                GLsizei height, GLint border, GLenum format, GLenum type,
                const GLvoid *data) noexcept;
  void texParameteri(GLenum name, GLint value) noexcept;
};

#endif // TEXTURE_H
