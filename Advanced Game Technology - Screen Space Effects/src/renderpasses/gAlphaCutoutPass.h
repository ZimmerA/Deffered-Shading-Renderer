#ifndef G_ALPHA_CUTOUT_PASS_H
#define G_ALPHA_CUTOUT_PASS_H

#include "managers/shaderManager.h"
#include "renderPass.h"
#include "scene.h"

class GAlphaCutoutPass : public RenderPass {
public:
  GAlphaCutoutPass(std::shared_ptr<FrameBuffer> frameBuffer,
            std::shared_ptr<ShaderManager> &shaderManager);
  void render(const RenderData &renderData, std::shared_ptr<Scene> &scene);

private:
  std::shared_ptr<ShaderProgram> gAlphaCutoutProgram;
};

#endif // G_ALPHA_CUTOUT_PASS_H