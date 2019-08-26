#include "renderBuffer.h"

void RenderBuffer::bind() noexcept {
  glBindRenderbuffer(GL_RENDERBUFFER, handle);
}

void RenderBuffer::unbind() noexcept { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

void RenderBuffer::renderBufferStorage(GLenum internalFormat, GLsizei width,
                                       GLsizei height) noexcept {
  bind();
  glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
  unbind();
}
