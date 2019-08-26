#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glTraits.h"
#include "glad.h"
#include "openGLObjectBase.h"
#include "texture.h"

class FrameBuffer : public OpenGLObjectBase<FrameBufferTraits> {
public:
  void bind() noexcept;
  static void bindDefault() noexcept;
  void attachTexture(GLenum attachment, GLuint textureTarget,
                     GLuint textureHandle, GLint level) noexcept;
  void attachRenderBuffer(GLenum target, GLenum attachment,
                          GLuint renderbuffer) noexcept;
  void setDrawBuffers(GLsizei n, const GLenum *bufs) noexcept;
};

#endif // FRAMEBUFFER_H
