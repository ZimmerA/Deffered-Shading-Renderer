#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "glTraits.h"
#include "openGLObjectBase.h"

class RenderBuffer : public OpenGLObjectBase<RenderBufferTraits> {
public:
  void bind() noexcept;
  void unbind() noexcept;
  void renderBufferStorage(GLenum internalFormat, GLsizei width,
                           GLsizei height) noexcept;
};

#endif // RENDERBUFFER_H