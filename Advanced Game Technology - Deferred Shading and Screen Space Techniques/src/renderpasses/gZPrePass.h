#ifndef G_Z_PRE_PASS_H
#define G_Z_PRE_PASS_H

#include "managers/shaderManager.h"
#include "renderPass.h"
#include "scene.h"

class GZPrePass : public RenderPass {
public:
  GZPrePass(std::shared_ptr<FrameBuffer> frameBuffer,
            std::shared_ptr<ShaderManager> &shaderManager);
  void render(const RenderData &renderData, std::shared_ptr<Scene> &scene);

private:
  std::shared_ptr<ShaderProgram> gPrePassProgram;
};

#endif // G_Z_PRE_PASS_H