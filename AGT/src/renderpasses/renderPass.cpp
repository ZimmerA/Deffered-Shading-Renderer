#include "renderPass.h"

void RenderPass::begin() noexcept {
  if (frameBuffer) {
    frameBuffer->bind();
  } else {
    FrameBuffer::bindDefault();
  }
}
