#include "frameBuffer.h"

void FrameBuffer::bind() noexcept { glBindFramebuffer(GL_FRAMEBUFFER, handle); }

void FrameBuffer::bindDefault() noexcept {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::attachTexture(GLenum attachment, GLuint textureTarget,
                                GLuint textureHandle, GLint level) noexcept {
  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget,
                         textureHandle, level);
}

void FrameBuffer::attachRenderBuffer(GLenum target, GLenum attachment,
                                     GLuint renderbuffer) noexcept {
  bind();
  glFramebufferRenderbuffer(target, attachment, GL_RENDERBUFFER, renderbuffer);
}

void FrameBuffer::setDrawBuffers(GLsizei n, const GLenum *bufs) noexcept {
  bind();
  glDrawBuffers(n, bufs);
}
