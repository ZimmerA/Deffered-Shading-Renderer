#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <memory>

#include "frameBuffer.h"

class RenderPass {
public:
  virtual void begin() noexcept;
  double result{};

protected:
  RenderPass(std::shared_ptr<FrameBuffer> frameBuffer) noexcept
      : frameBuffer(frameBuffer){};
  ~RenderPass() = default;
  std::shared_ptr<FrameBuffer> frameBuffer;
};

#endif // RENDERPASS_H